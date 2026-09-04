/****************************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
** All rights reserved.
**
****************************************************************************************/

#include "sailfish-webview-webengine.h"

// This file is used only for documentation purposes

namespace SailfishOS {

/*!
    \class SailfishOS::WebEngine
    \brief Provides access to the web engine context.
    \inmodule SailfishWebView
    \inherits QObject

    Singleton class which provides access to the global WebEngine context.
*/

/*!
    \brief Initializes the WebEngine class.

    The \a profilePath sets the root directory for the Gecko profile. Once set,
    it cannot be changed. Set \a runEmbedding to \c false to defer engine startup
    until \l runEmbedding is called.

    This method is called automatically when the WebView QML plugin is
    initialized. C++ applications may call it before loading QML when they need
    to control the profile location or initialization order. Multiple calls have
    no effect.

    \note Use a profile path that is accessible to the application. The
    \l{https://doc.qt.io/archives/qt-5.6/qstandardpaths.html}{standard application paths}
    are suitable for sandboxed applications.

    \sa instance, runEmbedding
*/
void SailfishOS::WebEngine::initialize(const QString &profilePath, bool runEmbedding);

/*!
    \brief Returns the singleton WebEngine instance.

    The returned instance may not yet be initialized.

    \sa initialize, initialized
*/
SailfishOS::WebEngine *SailfishOS::WebEngine::instance();

/*!
    \internal
    \brief Constructs a WebEngine with the specified \a parent.

    Applications should use \l instance instead of constructing a WebEngine.
*/
SailfishOS::WebEngine::WebEngine(QObject *parent);

/*!
    \internal
    \brief Destroys the WebEngine instance.
*/
SailfishOS::WebEngine::~WebEngine();

/*!
    \typedef SailfishOS::WebEngine::TaskCallback
    \brief Callback function invoked for a posted engine task.

    The callback receives the data pointer supplied when the task was posted.
*/

/*!
    \typedef SailfishOS::WebEngine::TaskHandle
    \brief Opaque handle for a posted engine task.

    A task handle may be passed to \l CancelTask before its callback runs.
*/

/*!
    \brief Returns whether hardware-accelerated rendering is enabled.

    Returns \c false if the engine backend is not available.

    \sa setIsAccelerated
*/
bool SailfishOS::WebEngine::isAccelerated() const;

/*!
    \brief Enables or disables hardware-accelerated rendering.

    The \a accelerated value is passed to the engine backend. The call has no
    effect if the backend is not available.

    \sa isAccelerated
*/
void SailfishOS::WebEngine::setIsAccelerated(bool accelerated);

/*!
    \brief Posts a callback to the WebEngine UI task queue.

    The \a callback receives \a data unchanged. If \a timeout is greater than
    zero, execution is delayed by that many milliseconds. The returned handle
    can be passed to \l CancelTask. A null handle indicates that the engine task
    queue is not available.

    \sa PostCompositorTask, CancelTask
*/
SailfishOS::WebEngine::TaskHandle SailfishOS::WebEngine::PostUITask(
        TaskCallback callback, void *data, int timeout);

/*!
    \brief Posts a callback to the WebEngine compositor task queue.

    The \a callback receives \a data unchanged. If \a timeout is greater than
    zero, execution is delayed by that many milliseconds. The returned handle
    can be passed to \l CancelTask. A null handle indicates that the compositor
    task queue is not available.

    \sa PostUITask, CancelTask
*/
SailfishOS::WebEngine::TaskHandle SailfishOS::WebEngine::PostCompositorTask(
        TaskCallback callback, void *data, int timeout);

/*!
    \brief Cancels a previously posted engine task.

    The \a handle must have been returned by \l PostUITask or
    \l PostCompositorTask. Passing a null handle has no effect.

    \sa PostUITask, PostCompositorTask
*/
void SailfishOS::WebEngine::CancelTask(TaskHandle handle);

/*!
    \brief Returns the number of Gecko windows currently registered with the
           WebEngine.

    This counts engine windows, not logical tabs hosted by those windows.
*/
int SailfishOS::WebEngine::getNumberOfWindows() const;

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

    Gecko is initialized on the Qt application thread and its work is
    integrated with the Qt event dispatcher. The retained \a aDelay argument
    is ignored for source compatibility.

    \sa {SailfishOS::WebEngine::initialize}{WebEngine::initialize}, stopEmbedding
*/
void SailfishOS::WebEngine::runEmbedding(int aDelay = -1);

/*!
    \brief Stops the engine event loop.

    Closes hosted windows and then terminates the WebEngine asynchronously.

    \sa {SailfishOS::WebEngine::initialize}{WebEngine::initialize}, runEmbedding
*/
void SailfishOS::WebEngine::stopEmbedding();

/*!
    \internal
    \brief Sends the "final-ui-startup" notification to observers

    For internal use during the Gecko engine startup.
*/
void SailfishOS::WebEngine::notifyFirstUIInitialized();

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
    \brief This signal is emitted when the last registered window has been destroyed.

    By default this is used to delay execution of \l stopEmbedding until after
    the last window has closed.
*/
void SailfishOS::WebEngine::lastWindowDestroyed();

/*!
    \brief Loads a user stylesheet into hosted web content.

    The \a url must identify a stylesheet. The stylesheet is registered with
    Gecko as a user stylesheet and applies to content hosted by this WebEngine.
*/
void SailfishOS::WebEngine::addUserStyleSheet(const QUrl &url);

/*!
    \brief Removes a previously loaded user stylesheet.
*/
void SailfishOS::WebEngine::removeUserStyleSheet(const QUrl &url);

/*!
    \property SailfishOS::WebEngine::initialized
    \brief Whether the context has been initialized

    \sa {SailfishOS::WebEngine::initialize}{WebEngine::initialize}
*/
bool SailfishOS::WebEngine::isInitialized() const;

} // namespace SailfishOS
