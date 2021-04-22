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

/*!
  \class SailfishOS::WebEngine
  \brief Provides access to the Web engine context
  \inmodule SailfishWebView

  Singleton class which provides access to the Web engine context.
*/

namespace SailfishOS {

/*!
  \brief Initialises the WebEngine class.

  Initalises the WebEngine class. The \a profilePath sets the root folder for
  the mozilla gecko profile. Once set the \a profilePath can't be changed.

  Multiple calls to initialize have no effect.

  \sa initialize
*/
void WebEngine::initialize(const QString &profilePath)
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

    WebEngine *webEngine = instance();
    webEngine->setProfile(profilePath);

    // Set various embedlite components
    webEngine->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/components/EmbedLiteBinComponents.manifest"));
    webEngine->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/components/EmbedLiteJSComponents.manifest"));
    webEngine->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/chrome/EmbedLiteJSScripts.manifest"));
    webEngine->addComponentManifest(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH + QString("/chrome/EmbedLiteOverrides.manifest"));

    QTimer::singleShot(0, webEngine, SLOT(runEmbedding()));

    isInitialized = true;
}

/*!
  \brief Returns the instance of the singleton WebEngine class.

  Returns the instance of the singleton WebEngine class.
  The returned instance may not be intialised.

  \sa initialize
*/
WebEngine *WebEngine::instance()
{
    return webEngineInstance();
}

/*!
  \brief Constructs a new WebEngine instance, with the specified QObject \a parent.

  In general there should be no need to use the constructor. Call \l instance
  to return the singleton instance instead.

  \sa instance
*/
WebEngine::WebEngine(QObject *parent)
    : QMozContext(parent)
{
}

/*!
  \brief Destroys the WebEngine instance.
*/
WebEngine::~WebEngine()
{
}

} // namespace SailfishOS
