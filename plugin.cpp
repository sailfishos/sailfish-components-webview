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

#include <QtCore/QTimer>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

//mozembedlite-qt5
#include <qmozcontext.h>
#define SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH QLatin1String("/usr/lib/mozembedlite/")

namespace SailfishOS {

namespace WebView {

static QMozContext* setupMozillaContext(const QString &userAgent = QLatin1String("Mozilla/5.0 (Maemo; Linux; U; Sailfish OS 2.0 (like Android 4.4); Mobile; rv:38.0) Gecko/38.0 Firefox/38.0 SailfishBrowser/1.0"), float pixelRatio = 2.0)
{
    // Workaround for https://bugzilla.mozilla.org/show_bug.cgi?id=929879
    setenv("LC_NUMERIC", "C", 1);
    setlocale(LC_NUMERIC, "C");

    setenv("USE_ASYNC", "1", 1);
    setenv("USE_NEMO_GSTREAMER", "1", 1);
    setenv("NO_LIMIT_ONE_GST_DECODER", "1", 1);

    // See https://www.freedesktop.org/wiki/Software/PulseAudio/Documentation/Developer/Clients/ApplicationProperties/
    setenv("PULSE_PROP_application.process.binary", qApp->applicationName().toUtf8(), 1);

    // GRE_HOME must be set before QMozContext is initialized. With invoker PWD is empty.
    QByteArray binaryPath = QCoreApplication::applicationDirPath().toLocal8Bit();
    setenv("GRE_HOME", binaryPath.constData(), 1);
    if (qgetenv("CUSTOM_UA").isEmpty()) {
        setenv("CUSTOM_UA", userAgent.toUtf8(), 1);
    }

    // initialize the QMozContext
    QMozContext *mozCtxt = QMozContext::GetInstance();

    // Set the pixel ratio
    bool ok = true;
    float envPixelRatio = qMax(QString::fromUtf8(qgetenv("CUSTOM_PIXEL_RATIO")).toFloat(&ok), 0.0f);
    if (!ok || qFuzzyCompare(envPixelRatio + 1.0d, 0.0f + 1.0d)) {
        mozCtxt->setPixelRatio(pixelRatio);
    } else {
        mozCtxt->setPixelRatio(envPixelRatio);
    }

    // Set various embedlite components
    mozCtxt->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/components/EmbedLiteBinComponents.manifest"));
    mozCtxt->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/components/EmbedLiteJSComponents.manifest"));
    mozCtxt->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/chrome/EmbedLiteJSScripts.manifest"));
    mozCtxt->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/chrome/EmbedLiteOverrides.manifest"));

    return mozCtxt;
}

void SailfishOSWebViewPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.WebView"));
    qmlRegisterType<SailfishOS::WebView::RawWebView>("Sailfish.WebView", 1, 0, "RawWebView");
}

void SailfishOSWebViewPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.WebView"));

    QMozContext *mozCtxt = setupMozillaContext();
    QObject::connect(mozCtxt, &QMozContext::onInitialized,
                     this, &SailfishOSWebViewPlugin::onMozillaContextInitialized);
    engine->rootContext()->setContextProperty("MozContext", mozCtxt);
    QTimer::singleShot(0, mozCtxt, SLOT(runEmbedding()));
    connect(engine, SIGNAL(destroyed()), mozCtxt, SLOT(stopEmbedding()));
}

void SailfishOSWebViewPlugin::onMozillaContextInitialized()
{
    QMozContext *mozCtxt = qobject_cast<QMozContext*>(sender());
    if (mozCtxt) {
        // For some yet unknown reason QmlMozView crashes when
        // flicking quickly if progressive-paint is enabled.
        mozCtxt->setPref("layers.progressive-paint", QVariant::fromValue<bool>(false));
        // Disable low-precision-buffer so that background underdraw works
        // correctly.
        mozCtxt->setPref("layers.low-precision-buffer", QVariant::fromValue<bool>(false));
        // Standard settings.
        mozCtxt->setPref("gfx.qt.rgb16.force", QVariant::fromValue<bool>(false));
        mozCtxt->setPref("embedlite.azpc.handle.singletap", QVariant::fromValue<bool>(false));
        mozCtxt->setPref("embedlite.azpc.json.singletap", QVariant::fromValue<bool>(true));
        mozCtxt->setPref("embedlite.azpc.handle.longtap", QVariant::fromValue<bool>(false));
        mozCtxt->setPref("embedlite.azpc.json.longtap", QVariant::fromValue<bool>(true));
        mozCtxt->setPref("embedlite.azpc.json.viewport", QVariant::fromValue<bool>(true));
        mozCtxt->setPref("apz.asyncscroll.throttle", QVariant::fromValue<int>(40));
        mozCtxt->setPref("apz.asyncscroll.timeout", QVariant::fromValue<int>(400));
        mozCtxt->setPref("apz.fling_stopped_threshold", QLatin1String("0.13f"));
    }
}

RawWebView::RawWebView(QQuickItem *parent)
    : QuickMozView(parent)
{
}

RawWebView::~RawWebView()
{
}

} // namespace WebView

} // namespace SailfishOS

