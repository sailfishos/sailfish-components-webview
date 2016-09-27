/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Chris Adams <chris.adams@jollamobile.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "plugin.h"
#include "webengine.h"
#include "webenginesettings.h"

#include <QtCore/QTimer>
#include <QtCore/QStandardPaths>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

#define SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH QLatin1String("/usr/lib/mozembedlite/")

namespace SailfishOS {

namespace WebView {

void SailfishOSWebViewPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.WebView"));
    qmlRegisterType<SailfishOS::WebView::RawWebView>("Sailfish.WebView", 1, 0, "RawWebView");
}

void SailfishOSWebViewPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.WebView"));

    AppTranslator *engineeringEnglish = new AppTranslator(engine);
    AppTranslator *translator = new AppTranslator(engine);
    engineeringEnglish->load("sailfish_components_webview_qt5_eng_en", "/usr/share/translations");
    translator->load(QLocale(), "sailfish_components_webview_qt5", "-", "/usr/share/translations");

    SailfishOS::WebEngine::initialize(QStandardPaths::writableLocation(QStandardPaths::CacheLocation),
                                      QLatin1String("Mozilla/5.0 (Maemo; Linux; U; Sailfish OS 2.0 (like Android 4.4); Mobile; rv:38.0) Gecko/38.0 Firefox/38.0 SailfishBrowser/1.0"));

    SailfishOS::WebEngine *webEngine = SailfishOS::WebEngine::instance();

    SailfishOS::WebEngineSettings::initialize();
    SailfishOS::WebEngineSettings *engineSettings = SailfishOS::WebEngineSettings::instance();

    // For some yet unknown reason QmlMozView crashes when
    // flicking quickly if progressive-paint is enabled.
    engineSettings->setPreference("layers.progressive-paint", QVariant::fromValue<bool>(false));
    // Disable low-precision-buffer so that background underdraw works
    // correctly.
    engineSettings->setPreference("layers.low-precision-buffer", QVariant::fromValue<bool>(false));

    // Don't expose any protocol handlers by default and don't warn about those.
    engineSettings->setPreference(QStringLiteral("network.protocol-handler.external-default"), false);
    engineSettings->setPreference(QStringLiteral("network.protocol-handler.expose-all"), false);
    engineSettings->setPreference(QStringLiteral("network.protocol-handler.warn-external-default"), false);

    // TODO : Stop embedding after lastWindow is destroyed.
    connect(engine, SIGNAL(destroyed()), webEngine, SLOT(stopEmbedding()));
}

RawWebView::RawWebView(QQuickItem *parent)
    : QuickMozView(parent)
    , m_flickable(0)
    , m_startPos(-1.0, -1.0)
    , m_movePos(-1.0, -1.0)
{
}

RawWebView::~RawWebView()
{
}

QQuickItem *RawWebView::flickable() const
{
    return m_flickable;
}

void RawWebView::setFlickable(QQuickItem *flickable)
{
    if (m_flickable != flickable) {
        m_flickable = flickable;
        if (m_flickable) {
            m_flickable->installEventFilter(this);
        }
        emit flickableChanged();
    }
}

// This event filter exists to handle the case where the WebView
// is within a WebViewFlickable, and the user has panned the WebView
// to atYBeginning() and there is a PullDownMenu available.
// In this case, the WebViewFlickable will be interactive (and thus
// steal touch events), but in the case that the user attempts to
// pan the WebView downwards, we want to let the WebView rather than
// the WebViewFlickable handle the touch event.
// (Similarly for when the WebView is atYEnd() with a PushUpMenu.)
bool RawWebView::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::TouchBegin ||
            event->type() == QEvent::TouchCancel ||
            event->type() == QEvent::TouchEnd ||
            event->type() == QEvent::TouchUpdate) {
qWarning() << "received touch event!!";
        if (m_flickable->property("interactive").toBool() && (atYBeginning() || atYEnd())) {
qWarning() << "and the flickable is interactive, and we're atYBeginning() or atYEnd()";
            QTouchEvent *te = static_cast<QTouchEvent *>(event);
            if (te->touchPoints().size() == 1) {
                const QTouchEvent::TouchPoint &tp = te->touchPoints().at(0);
                QPointF currentPos = tp.pos();
                if ((m_startPos.x() != -1.0) && (m_startPos.y() != -1.0)             // ignore the first touch event
                        && ((atYBeginning() && currentPos.y() < m_startPos.y()) ||   // if we're at the top and the user is panning down
                            (atYEnd() && currentPos.y() > m_startPos.y()))) {        // or we're at the bottom and the user is panning up
qWarning() << "and the user is panning away from the pulley manu!  synthesising a touch point at:" << currentPos.x() << "," << currentPos.y();
                    // pass the touch through to the underlying webview.
                    QList<QVariant> list;
                    list.append(QVariant::fromValue<QPointF>(QPointF(currentPos.x(), currentPos.y())));
                    if (m_movePos.x() == -1.0 && m_movePos.y() == -1.0) {
                        synthTouchBegin(list);
                        m_movePos = currentPos;
                    } else {
                        // hopefully this doesn't overwrite the binding...
                        m_flickable->setProperty("interactive", QVariant::fromValue<bool>(false));
                        synthTouchMove(list);
                        m_startPos = QPointF(-1.0, -1.0);
                        m_movePos = QPointF(-1.0, -1.0);
                    }
                    return true;
                } else {
                    m_startPos = currentPos;
                }
            }
        }
    }
    return false;
}

} // namespace WebView

} // namespace SailfishOS

