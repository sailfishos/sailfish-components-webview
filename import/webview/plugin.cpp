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

#include <QQuickWindow>

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
    , m_vkbMargin(0.0)
{
    connect(qGuiApp->inputMethod(), &QInputMethod::visibleChanged, this, [=]() {
        if (qGuiApp->inputMethod()->isVisible()) {
            setFollowItemGeometry(false);
        }
    });
}

RawWebView::~RawWebView()
{
}

qreal RawWebView::virtualKeyboardMargin() const
{
    return m_vkbMargin;
}

void RawWebView::setVirtualKeyboardMargin(qreal vkbMargin)
{
    if (m_vkbMargin != vkbMargin) {
        m_vkbMargin = vkbMargin;
        QMargins margins;
        margins.setBottom(m_vkbMargin);
        setMargins(margins);
        emit virtualKeyboardMarginChanged();

        QVariantMap map;
        map.insert("imOpen", m_vkbMargin > 0);
        map.insert("pixelRatio", SailfishOS::WebEngineSettings::instance()->pixelRatio());
        map.insert("bottomMargin", m_vkbMargin);
        map.insert("screenWidth", window()->width());
        map.insert("screenHeight", window()->height());
        QVariant data(map);
        sendAsyncMessage("embedui:vkbOpenCompositionMetrics", data);

        if (m_vkbMargin == 0) {
            setFollowItemGeometry(true);
        }
    }
}

} // namespace WebView

} // namespace SailfishOS

