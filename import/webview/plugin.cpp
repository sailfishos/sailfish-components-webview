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
#include "rawwebview.h"
#include "webengine.h"
#include "webenginesettings.h"

#include <QtCore/QStandardPaths>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

#include <QQuickWindow>
#include <QClipboard>
#include <QGuiApplication>

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
    webEngine->addObservers({ QStringLiteral("clipboard:setdata") });
}

} // namespace WebView

} // namespace SailfishOS

