/****************************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
** All rights reserved.
**
****************************************************************************************/

#include "sailfish-webview-webengine.h"

// This file is used only for documentation purposes

// These items are all part of QMozContext, but inherited by WebEngine, so we
// take some notational liberty and present them as part of the subclass.

namespace SailfishOS {

/*!
    \brief Registers the WebEngine for receiving notifications on a topic.

    The Gecko engine and EmbedLite components use notifications to transfer data
    and information about events to other components. The \c addObserver method
    allows the WebEngine to be registered as interested in the particular \a
    aTopic so that it will start receiving notifications about them.

    When WebEngine receives a notification on a registered \a aTopic it will emit
    a \c recvObserve signal.

    The Mozilla \l {https://developer.mozilla.org/en-US/docs/Mozilla/Tech/XPCOM/Reference/Interface/nsIObserverService}{nsIObserver}
    documentation provides details about the underlying processes.

    There is also a non-exhaustive list of \l {https://developer.mozilla.org/en-US/docs/Mozilla/Tech/XPCOM/Observer_Notifications}{observer notification topics}
    that can be subscribed to.

    \sa addObservers, removeObserver, removeObservers, recvObserve, notifyObservers
*/
void SailfishOS::WebEngine::addObserver(const QString &aTopic);

/*!
    \brief Registers the WebEngine for receiving notifications on multiple topics.

    The \c addObservers method should be used to register interest in multiple
    topics simultaneously. The \a aObserversList should be set to contain a list
    of all the topics of interest.

    This is equivalent to calling \l addObserver multiple times.

    See \l addObserver for more detailed info about notifications and observers.

    \sa addObserver, removeObserver, removeObservers, recvObserve, notifyObservers
*/
void SailfishOS::WebEngine::addObservers(const std::vector<std::string> &aObserversList);

/*!
    \brief Unregisters the WebEngine from receiving notifications on a topic.

    If a component which previously registered an interest in receiving
    notifications on \a aTopic by calling \l addObserver is no longer interested
    in receiving them, it can call \c removeObserver to indicate this.

    Calling \c removeObserver does not guarantee that no more notifications on
    the topic will be received, since other components may have registered an
    interest with the \c WebEngine. The \c WebEngine will stop emitting signals
    on the topic only once all interests have been deregistered.

    Only topics that have previously been registered using \l addObserver or
    \l addObservers should be unregistered using \l removeObserver.

    See \l addObserver for more detailed info about notifications and observers.

    \sa removeObservers, addObserver, addObservers, recvObserve, notifyObservers
*/
void SailfishOS::WebEngine::removeObserver(const QString  aTopic);

/*!
    \brief Unregisters the WebEngine from receiving notifications on multiple topics.

    The \c removeObservers method should be used to unregister interest in multiple
    topics simultaneously. The \a aObserversList should be set to contain a list
    of all the topics that are no longer of interest.

    Only topics that have previously been registered using \l addObserver or
    \l addObservers should be included in the \a aObserversList.

    This is equivalent to calling \l removeObserver multiple times.

    See \l addObserver for more detailed info about notifications and observers.

    \sa addObservers, removeObserver, removeObservers, recvObserve, notifyObservers
*/
void SailfishOS::WebEngine::removeObservers(const std::vector<std::string> &aObserversList);

/*!
    \brief Sends a broadcast notification that can be observed by other components.

    Sends a notification that will be picked up by any component that has
    registered an observer for \a topic.

    Additional data can be sent with the notification using the \a value
    parameter encoded as a JSON string.

    \sa addObserver, recvObserve
*/
void SailfishOS::WebEngine::notifyObservers(const QString &topic, const QString &value);

/*!
    \brief Sends a broadcast notification that can be observed by other components.

    Sends a notification that will be picked up by any component that has
    registered an observer for \a topic.

    Additional data can be sent with the notification using the \a value
    parameter which should contain a structure (e.g. a \c{QVariantMap}) that will
    be encoded as a JSON object.

    \sa addObserver, recvObserve
*/
void SailfishOS::WebEngine::notifyObservers(const QString &topic, const QVariant &value);

/*!
    \brief Starts the engine event loop.

    When WebEngine is instantiated as a QML component this is called
    automatically during initialization. However, it can be called earlier to
    set the WebEngine event loop running in case the initialization process is
    being overridden.

    See \l {SailfishOS::WebEngine::initialize}{WebEngine::initialize} for more info.

    The \a aDelay allows for an asynchronous startup. Calling without the
    \a aDelay parameter (so it defaults to \c{-1}) will cause execution to
    happen on the nested main loop, which will block until \l stopEmbedding is
    called.

    \sa {SailfishOS::WebEngine::initialize}{WebEngine::initialize}, stopEmbedding, setProfile
*/
void SailfishOS::WebEngine::runEmbedding(int aDelay = -1);

/*!
    \brief Stops the engine event loop.

    Causes execution of the WebEngine to halt. If \l runEmbedding was called
    without the \c aDelay parameter, this will cause the \l runEmbedding call
    to unblock.

    \sa {SailfishOS::WebEngine::initialize}{WebEngine::initialize}, runEmbedding, setProfile
*/
void SailfishOS::WebEngine::stopEmbedding();

/*!
    \internal
    \brief Sends the "final-ui-startup" notification to observers

    For internal use during the Gecko engine startup.
*/
void SailfishOS::WebEngine::notifyFirstUIInitialized();

/*!
    \brief Sets the location for the profile directory.

    This is called during the startup process to set \a profilePath as the
    location to store the profile (inside which the \c{.mozilla} directory will
    be created). By default this is set to
    \c QStandardPaths::writableLocation(QStandardPaths::CacheLocation), which
    is usually \c{~/.cache/<organisation name>/<application name>}.

    Generally there is no need to call \c setProfile as the profile will
    automatically set up on initialization, and can also be passed as
    parameter to \l{SailfishOS::WebEngine::initialize}{WebEngine::initialize}.

    When WebEngine is instantiated as a QML component this is also called
    automatically during initialization. However, it can be called earlier to
    set a custom profile location in case the initialization process is being
    overridden.

    \note If using a different location care is needed to select a path that's
    acceptable from an application sandboxing point of view. The
    \l{https://doc.qt.io/archives/qt-5.6/qstandardpaths.html}{standard application paths}
    are safe for profile paths.

    See \l {SailfishOS::WebEngine::initialize}{WebEngine::initialize} for more
    info.

    \sa {SailfishOS::WebEngine::initialize}{WebEngine::initialize}, runEmbedding, stopEmbedding
*/
void SailfishOS::WebEngine::setProfile(const QString &profilePath);

/*!
    \internal
    \brief Called automatically on WebView creation so that the engine can create views.

    For internal use.

    \sa setViewCreator
*/
void SailfishOS::WebEngine::setViewCreator(QMozViewCreator *viewCreator);

/*!
    \internal
    \brief Opens a new view that can contain webcontent.

    For internal use.

    \sa setViewCreator
*/
void SailfishOS::WebEngine::createView();

/*!
    \brief This signal is emitted when a notification on an observed topic is received.

    After an app has registered an interest in a particular topic by calling
    \l addObserver or \l addObservers, it can then connect to this signal to be
    notified about any component sending a notification on the topic.

    The \a message parameter will be the topic in question, the \a data parameter
    will contain data sent by the sender constructed from a JSON object. The
    exact object structure is at the sender's discretion.

    \sa addObserver, addObservers, removeObserver, removeObservers, notifyObservers
*/
void SailfishOS::WebEngine::recvObserve(const QString message, const QVariant data);

/*!
    \brief This signal is emitted after the embedding has been
           \l{stopEmbedding}{stopped} and the context is ready to be deleted.

    This signal is emitted when the embedding has been
    \l{stopEmbedding}{stopped}, just prior to the WebEngine being deleted and
    the program exiting.
*/
void SailfishOS::WebEngine::contextDestroyed();

/*!
    \brief This signal is emitted when when the last view has been destroyed.
*/
void SailfishOS::WebEngine::lastViewDestroyed();

/*!
    \brief This signal is emitted when the last registered window has been destroyed.

    By default this is used to delay execution of \l stopEmbedding until after
    the last window has closed.
*/
void SailfishOS::WebEngine::lastWindowDestroyed();

/*!
    \brief Register JavaScript chrome components to be loaded into the WebEngine.

    The Sailfish WebEngine has three main layers. The top layer is exposed as
    QML components with a developer-facing interface. The lowest level is the
    native Gecko rendering engine.

    Between the two sits a JavaScript layer that can communicate with either
    of the other layers using an observer/notification pattern. These
    JavaScript components are considered "privileged code" and run with the
    \l {https://developer.mozilla.org/en-US/docs/Mozilla/Gecko/Script_security#security_principals}{system principal}.

    This call allows new JavaScript components to be added and executed as part
    of this intermediary layer.

    The \a manifestPath should be an absolute path pointing to a manifest file.
    The manifest file should contain references to components in the same folder,
    as shown in the following example. The example is comprised of three files:
    the main app initialisation code that calls \c addComponentManifest, the
    manifest file and a component referenced in the manifest. The would usually
    also be a frontend QML file containing a \c WebView component which is
    omitted for brevity.

    The app \b main entry point which calls \c addComponentManifest.

    \code
        #include <QtQuick>
        #include <sailfishapp.h>
        #include <libsailfishwebengine/webengine.h>

        int main2(int argc, char *argv[])
        {
            QGuiApplication *app = SailfishApp::application(argc, argv);
            QQuickView *view = SailfishApp::createView();
            view->setSource(SailfishApp::pathTo("application.qml"));

            SailfishOS::WebEngine *webEngine = SailfishOS::WebEngine::instance();
            webEngine->addComponentManifest(QLatin1String("/path/file.manifest"));

            view->show();

            return app->exec();
        }
    \endcode

    The \b manifest file at /path/file.manifest

    \code
        component {20227a22-1722-4753-b8f6-c842b401b4c3} ExampleComponent.js
        contract @mozilla.org/embedlite-example-component;1 {20227a22-1722-4753-b8f6-c842b401b4c3}
        category app-startup ExampleComponent service,@mozilla.org/embedlite-example-component;1
    \endcode

    The JavaScript \b component referenced in the manifest and stored at
    /path/ExampleComponent.js

    \code
        const Ci = Components.interfaces;
        const Cu = Components.utils;

        Cu.import("resource://gre/modules/XPCOMUtils.jsm");
        Cu.import("resource://gre/modules/Services.jsm");

        XPCOMUtils.defineLazyServiceGetter(Services, "embedlite",
                                            "@mozilla.org/embedlite-app-service;1",
                                            "nsIEmbedAppService");

        Services.scriptloader.loadSubScript("chrome://embedlite/content/Logger.js");

        function ExampleComponent() {
          Logger.debug("JSComp: ExampleComponent.js loaded");
        }

        ExampleComponent.prototype = {
          classID: Components.ID("{20227a22-1722-4753-b8f6-c842b401b4c3}"),

          QueryInterface: XPCOMUtils.generateQI([Ci.nsIObserver]),

          observe: function(aSubject, aTopic, aData) {
            switch (aTopic) {
            case "app-startup":
              Services.obs.addObserver(this, "exampleTopic", false);
              break;
            case "exampleTopic":
              Logger.debug("ExampleComponent: exapleTopic data: " + aData);
              break;
            }
          },
        };

        this.NSGetFactory = XPCOMUtils.generateNSGetFactory([ExampleComponent]);
    \endcode

    The example simply prints various debug messages to indicate the component
    has been initialized and when an "exampleTopic" message is received. There
    are many internal Gecko components with public interfaces that can't be
    accessed directly from QML, but can be accessed by a component installed
    in this way.

    A common pattern is therefore for an app using a web view to install a
    component to act as an intermediary. The QML interface sends a message
    to the component, which calls the internal Gecko interface, and then
    sends back the result in a message that's picked up by the front end.

    \note In this context "privileged" and "system principal" are
    \l{https://developer.mozilla.org/en-US/docs/Mozilla/Gecko/Script_security}{gecko terminology}.
    They don't relate to any system privileges external to the web view.
*/
void SailfishOS::WebEngine::addComponentManifest(const QString &manifestPath);

/*!
    \property SailfishOS::WebEngine::initialized
    \brief Whether the context has been initialized

    \sa {SailfishOS::WebEngine::initialize}{WebEngine::initialize}
*/
bool SailfishOS::WebEngine::isInitialized();

} // namespace SailfishOS
