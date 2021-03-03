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
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

#include <QQuickWindow>
#include <QClipboard>
#include <QGuiApplication>
#include <QDir>
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace SailfishOS {

namespace WebView {

const auto MOZILLA_DATA_UA_UPDATE = QStringLiteral("ua-update.json");
const auto MOZILLA_DATA_UA_UPDATE_SOURCE = QStringLiteral("/usr/share/sailfish-browser/data/ua-update.json.in");

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

    // TODO : Stop embedding after lastWindow is destroyed.
    connect(engine, SIGNAL(destroyed()), webEngine, SLOT(stopEmbedding()));

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

