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
    \brief Provides access to the web engine context.
    \inmodule SailfishWebView
    \inherits QMozContext

    Singleton class which provides access to the web engine context.
*/

namespace SailfishOS {

/*!
    \brief Initialises the WebEngine class.

    Initalizes the WebEngine class. The \a profilePath sets the root folder for
    the mozilla gecko profile. Once set the \a profilePath can't be changed.
    Set \a runEmbedding to \c false (defaults to true) in order to handle steps
    that are needed before \l{runEmbedding}{engine startup}.

    Multiple calls to initialize have no effect.

    This method will be called automatically during QML initialisation of the
    WebView. However under some circumstances it may be useful to perform a
    custom initalization. In this case \c initialize can be called manually as
    long as this is done before the QML engine initialization.

    One reason for custom initialization would be to provide additional
    component manifests.

    \code
        SailfishOS::WebEngine *webEngine = SailfishOS::WebEngine::instance();
        QString profilePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        webEngine->initialize(profilePath, false);
        webEngine->addComponentManifest(QLatin1String("/path/file.manifest"));

        [...]

        QTimer::singleShot(0, webEngine, SLOT(runEmbedding()));
    \endcode

    \note Using component manifests in this way should be avoided in favour of
    frame scripts where possible. Frame scritps are likely to cover the
    majority of cases.

    \note By default When instantiated from QML \a profilePath is set to
    \c QStandardPaths::writableLocation(QStandardPaths::CacheLocation), which
    is by default \c{~/.cache/<organisation name>/<application name>}. If using
    a different location care is needed to select a path that's acceptable from
    an application sandboxing point of view. The
    \l{https://doc.qt.io/archives/qt-5.6/qstandardpaths.html}{standard application paths}
    are safe for profile paths.

    \sa setProfile, addComponentManifest, runEmbedding
*/
void WebEngine::initialize(const QString &profilePath, bool runEmbedding)
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

    if (runEmbedding) {
        QTimer::singleShot(0, webEngine, SLOT(runEmbedding()));
    }

    isInitialized = true;
}

/*!
    \brief Returns the instance of the singleton WebEngine class.

    The returned instance may not be initialized.

    \sa initialize
*/
WebEngine *WebEngine::instance()
{
    return webEngineInstance();
}

/*!
    \internal
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
    \internal
    \brief Destroys the WebEngine instance.

    WebEngine is a singleton, so should never be deleted by the client. It will
    be automatically deleted at application shutdown.
*/
WebEngine::~WebEngine()
{
}

} // namespace SailfishOS
