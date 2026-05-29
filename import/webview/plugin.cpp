/****************************************************************************
**
** Copyright (C) 2021 Jolla Ltd.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "plugin.h"
#include "rawwebview.h"
#include "webengine.h"
#include "webenginesettings.h"

#include <QtCore/QStandardPaths>
#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>
#include <QtCore/QEventLoop>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

#include <QQuickWindow>
#include <QWindow>
#include <QClipboard>
#include <QGuiApplication>
#include <QDir>
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <qmozsecurity.h>
#include <qmozscrolldecorator.h>
#include <qmozcontext.h>

namespace SailfishOS {

namespace WebView {

namespace {

class WebViewShutdownController : public QObject
{
public:
    explicit WebViewShutdownController(SailfishOS::WebEngine *webEngine, QObject *parent)
        : QObject(parent)
        , m_webEngine(webEngine)
    {
        connect(webEngine, &SailfishOS::WebEngine::contextDestroyed, this, [this]() {
            m_contextDestroyed = true;
            if (m_shutdownWatchdog.isActive()) {
                m_shutdownWatchdog.stop();
            }
            if (m_waitLoop) {
                m_waitLoop->quit();
            }
            if (m_quitAfterShutdown) {
                restoreQuitOnLastWindowClosed();
                QCoreApplication::quit();
            }
        });
        if (qGuiApp) {
            connect(qGuiApp, &QGuiApplication::lastWindowClosed, this, [this]() {
                scheduleShutdown(true);
            });
        }
        connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [this]() {
            shutdownAndWait();
        });
        m_shutdownWatchdog.setSingleShot(true);
        connect(&m_shutdownWatchdog, &QTimer::timeout, this, [this]() {
            qWarning() << "Timed out waiting for WebEngine shutdown";
            restoreQuitOnLastWindowClosed();
        });
    }

    void watchEngine(QQmlEngine *engine)
    {
        connect(engine, &QObject::destroyed, this, [this]() {
            scheduleShutdown(false);
        });
    }

    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (event->type() == QEvent::Close) {
            QWindow *window = qobject_cast<QWindow *>(object);
            if (window && isLastVisibleWindow(window)) {
                scheduleShutdown(true);
            }
        }

        return QObject::eventFilter(object, event);
    }

private:
    bool isLastVisibleWindow(QWindow *closingWindow) const
    {
        const QList<QWindow *> windows = QGuiApplication::topLevelWindows();
        for (QWindow *window : windows) {
            if (window != closingWindow && window->isVisible()) {
                return false;
            }
        }

        return true;
    }

    void disableQuitOnLastWindowClosed()
    {
        if (!qGuiApp || m_savedQuitOnLastWindowClosed) {
            return;
        }

        m_quitOnLastWindowClosed = qGuiApp->quitOnLastWindowClosed();
        m_savedQuitOnLastWindowClosed = true;
        if (m_quitOnLastWindowClosed) {
            qGuiApp->setQuitOnLastWindowClosed(false);
        }
    }

    void restoreQuitOnLastWindowClosed()
    {
        if (!qGuiApp || !m_savedQuitOnLastWindowClosed) {
            return;
        }

        qGuiApp->setQuitOnLastWindowClosed(m_quitOnLastWindowClosed);
        m_savedQuitOnLastWindowClosed = false;
    }

    void scheduleShutdown(bool quitAfterShutdown)
    {
        if (m_shutdownScheduled || m_shutdownStarted || m_shutdownInProgress
                || m_contextDestroyed || !m_webEngine) {
            return;
        }

        m_quitAfterShutdown = quitAfterShutdown;
        if (quitAfterShutdown) {
            disableQuitOnLastWindowClosed();
        }

        m_shutdownScheduled = true;
        QTimer::singleShot(0, this, [this]() {
            shutdown();
        });
    }

    bool hasGeckoViews() const
    {
        return QMozContext::instance()->getNumberOfViews() != 0;
    }

    void destroyViewsAndWait()
    {
        if ((!RawWebView::hasLiveViews() && !hasGeckoViews()) || !m_webEngine) {
            return;
        }

        QEventLoop waitLoop;
        QTimer watchdog;
        watchdog.setSingleShot(true);
        connect(&watchdog, &QTimer::timeout, &waitLoop, &QEventLoop::quit);
        connect(m_webEngine, &SailfishOS::WebEngine::lastViewDestroyed,
                &waitLoop, &QEventLoop::quit);

        RawWebView::destroyLiveViews();
        QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        if (RawWebView::hasLiveViews() || hasGeckoViews()) {
            watchdog.start(2000);
            waitLoop.exec(QEventLoop::ExcludeUserInputEvents);
            QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        if (RawWebView::hasLiveViews() || hasGeckoViews()) {
            qWarning() << "Timed out waiting for WebView items to be destroyed";
        }
    }

    void startShutdown()
    {
        if (m_shutdownStarted || m_contextDestroyed || !m_webEngine) {
            return;
        }

        m_shutdownStarted = true;
        m_webEngine->stopEmbedding();
    }

    void shutdown()
    {
        if (m_shutdownInProgress || m_contextDestroyed || !m_webEngine) {
            return;
        }

        m_shutdownScheduled = false;
        m_shutdownInProgress = true;
        destroyViewsAndWait();
        startShutdown();
        if (!m_contextDestroyed) {
            m_shutdownWatchdog.start(5000);
        }
        m_shutdownInProgress = false;
    }

    void waitForContextDestroyed()
    {
        if (m_contextDestroyed || !m_webEngine) {
            return;
        }

        QEventLoop waitLoop;
        QTimer watchdog;
        watchdog.setSingleShot(true);
        connect(&watchdog, &QTimer::timeout, &waitLoop, &QEventLoop::quit);
        connect(m_webEngine, &SailfishOS::WebEngine::contextDestroyed,
                &waitLoop, &QEventLoop::quit);

        m_waitLoop = &waitLoop;
        watchdog.start(5000);
        waitLoop.exec(QEventLoop::ExcludeUserInputEvents);
        m_waitLoop = nullptr;

        if (!m_contextDestroyed) {
            qWarning() << "Timed out waiting for WebEngine shutdown";
        }
    }

    void shutdownAndWait()
    {
        if (m_shutdownInProgress || m_contextDestroyed || !m_webEngine) {
            return;
        }

        m_shutdownInProgress = true;
        destroyViewsAndWait();
        startShutdown();
        waitForContextDestroyed();
        restoreQuitOnLastWindowClosed();
        m_shutdownInProgress = false;
    }

private:
    QPointer<SailfishOS::WebEngine> m_webEngine;
    QTimer m_shutdownWatchdog;
    QEventLoop *m_waitLoop = nullptr;
    bool m_shutdownStarted = false;
    bool m_shutdownInProgress = false;
    bool m_shutdownScheduled = false;
    bool m_quitAfterShutdown = false;
    bool m_savedQuitOnLastWindowClosed = false;
    bool m_quitOnLastWindowClosed = true;
    bool m_contextDestroyed = false;
};

WebViewShutdownController *shutdownController(SailfishOS::WebEngine *webEngine)
{
    static QPointer<WebViewShutdownController> controller;
    if (!controller) {
        controller = new WebViewShutdownController(webEngine, QCoreApplication::instance());
        QCoreApplication::instance()->installEventFilter(controller);
    }
    return controller;
}

}

const auto MOZILLA_DATA_UA_UPDATE = QStringLiteral("ua-update.json");
const auto MOZILLA_DATA_UA_UPDATE_SOURCE = QStringLiteral("/usr/share/sailfish-browser/data/ua-update.json.in");

void SailfishOSWebViewPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.WebView"));
    qmlRegisterType<SailfishOS::WebView::RawWebView>("Sailfish.WebView", 1, 0, "RawWebView");
    // RawWebView inherits QuickMozView which has some pointer typed properties which need some extra
    // registration. a bit hazy where these should be really registered but here it works,
    // on QuickMozView ctor it doesn't
    qmlRegisterType<QMozSecurity>();
    qmlRegisterType<QMozScrollDecorator>();
}

void SailfishOSWebViewPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.WebView"));

    AppTranslator *engineeringEnglish = new AppTranslator(engine);
    AppTranslator *translator = new AppTranslator(engine);
    engineeringEnglish->load("sailfish_components_webview_qt5_eng_en", "/usr/share/translations");
    translator->load(QLocale(), "sailfish_components_webview_qt5", "-", "/usr/share/translations");

    QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    initUserAgentOverrides(path);
    SailfishOS::WebEngine::initialize(path);

    SailfishOS::WebEngine *webEngine = SailfishOS::WebEngine::instance();

    SailfishOS::WebEngineSettings::initialize();
    SailfishOS::WebEngineSettings *engineSettings = SailfishOS::WebEngineSettings::instance();

    // For some yet unknown reason QmlMozView crashes when
    // flicking quickly if progressive-paint is enabled.
    engineSettings->setPreference("layers.progressive-paint", QVariant::fromValue<bool>(false));
    // Disable low-precision-buffer so that background underdraw works
    // correctly.
    engineSettings->setPreference("layers.low-precision-buffer", QVariant::fromValue<bool>(false));

    shutdownController(webEngine)->watchEngine(engine);

    connect(webEngine, &SailfishOS::WebEngine::recvObserve, [](const QString &message, const QVariant &data) {
        const QVariantMap dataMap = data.toMap();
        if (message == QLatin1String("clipboard:setdata")) {
            QClipboard *clipboard = QGuiApplication::clipboard();

            // check if we copied password
            if (!dataMap.value(QStringLiteral("private")).toBool()) {
                clipboard->setText(dataMap.value(QStringLiteral("data")).toString());
            }
        }
    });

    // subscribe to gecko messages
    webEngine->addObserver(QStringLiteral("clipboard:setdata"));
}

void SailfishOSWebViewPlugin::initUserAgentOverrides(const QString &path)
{
    // Read source json
    QJsonDocument sourceDoc;
    QFile sourceFile(MOZILLA_DATA_UA_UPDATE_SOURCE);
    if (sourceFile.open(QIODevice::ReadOnly)) {
        QByteArray line;
        QByteArray sourceData;
        while (!sourceFile.atEnd()) {
            line = sourceFile.readLine();
            if (!line.trimmed().startsWith("//"))
                sourceData += line;
        }
        sourceFile.close();
        QJsonParseError e;
        sourceDoc = QJsonDocument::fromJson(sourceData, &e);
        if (sourceDoc.isNull()) {
            qWarning() << sourceFile.fileName() << "parse error" << e.errorString() << "at offset" << e.offset;
            return;
        }
    } else {
        qWarning() << "Could not open" << sourceFile.fileName();
    }

    // Make the mozilla dir if not there
    QString mozillaDir = QString("%1/.mozilla/").arg(path);
    QDir dir(mozillaDir);
    if (!dir.exists())
        dir.mkpath(dir.path());

    // Read destination json
    QJsonDocument destDoc;
    QFile destFile(mozillaDir + MOZILLA_DATA_UA_UPDATE);
    if (destFile.open(QIODevice::ReadWrite)) {
        QJsonParseError e;
        destDoc = QJsonDocument::fromJson(destFile.readAll(), &e);
        if (destDoc.isNull()) {
            qWarning() << destFile.fileName() << "parse error" << e.errorString() << "at offset" << e.offset;
            // Not bailing out to get the corrupted file rewritten
        }
        QJsonObject source = sourceDoc.object();
        QJsonObject dest = destDoc.object();
        QStringList destKeys = dest.keys();
        bool changed = false;
        for (const QString &key : source.keys()) {
            if (!destKeys.contains(key) || (dest.value(key) != source.value(key))) {
                // Add or change override
                dest.insert(key, source.value(key));
                changed = true;
            }
        }
        if (changed) {
            // Content changed so write it
            destFile.seek(0);
            destFile.write(sourceDoc.toJson());
        }
        destFile.close();
    } else {
        qWarning() << "Could not open" << destFile.fileName();
    }
}

} // namespace WebView

} // namespace SailfishOS
