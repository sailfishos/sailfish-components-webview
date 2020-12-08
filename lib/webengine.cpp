/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Raine Makelainen <raine.makelaine@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "webengine.h"

#include <QCoreApplication>
#include <QTimer>

Q_GLOBAL_STATIC(SailfishOS::WebEngine, webEngineInstance)

void SailfishOS::WebEngine::initialize(const QString &profilePath)
{
    static bool isInitialized = false;
    if (isInitialized) {
        return;
    }

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

    SailfishOS::WebEngine *webEngine = instance();
    webEngine->setProfile(profilePath);

    // Set various embedlite components
    webEngine->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/components/EmbedLiteBinComponents.manifest"));
    webEngine->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/components/EmbedLiteJSComponents.manifest"));
    webEngine->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/chrome/EmbedLiteJSScripts.manifest"));
    webEngine->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/chrome/EmbedLiteOverrides.manifest"));

    QTimer::singleShot(0, webEngine, SLOT(runEmbedding()));

    isInitialized = true;
}

SailfishOS::WebEngine *SailfishOS::WebEngine::instance()
{
    return webEngineInstance();
}

SailfishOS::WebEngine::WebEngine(QObject *parent)
    : QMozContext(parent)
{
}

SailfishOS::WebEngine::~WebEngine()
{
}
