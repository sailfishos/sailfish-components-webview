import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.WebView 1.0

SilicaFlickable {
    id: flickable

    boundsBehavior: Flickable.DragOverBounds
    interactive: (flickable.pullDownMenu != null && flickable.pullDownMenu.active)
              || (flickable.pushUpMenu != null && flickable.pushUpMenu.active)
              || ((flickTimer.showPullDownMenu || flickTimer.showPushUpMenu) && verticalVelocity <= 0 && !flickTimer.running)

    onVerticalVelocityChanged: {
        if (!flickTimer.running
                && verticalVelocity != 0
                && (flickable.pullDownMenu == null || !flickable.pullDownMenu.active)
                && (flickable.pushUpMenu == null || !flickable.pushUpMenu.active)) {
            // This is triggered when the pulley menu is interactive, but the user is
            // attempting to pan the webview "away" from the pulley menu.
            // (That is, either the current location is the top, and therefore the
            // pull-down menu is interactive, but the user has attempted to pan downards,
            // or the current location is the bottom, and therefore the push-up menu is
            // interactive, but the user has attempted to pan upwards.)
            //
            // In this case, we would ideally like to "pass" the touch event directly
            // to the WebView to handle as a panning gesture.  However, I don't know
            // how to get the touch coordinate from the Flickable, else I guess we
            // could just do:
            // if (flickTimer.webView !== null) {
            //   flickTimer.webView.synthTouchBegin([Qt.point(mouse.x,mouse.y)]);
            // }
            //
            // As a hacky workaround, we disable flickable interaction for a brief
            // period of time, giving the user a window of time within which to pan away,
            // since touch events will not be delivered to the flickable but to the webview,
            // however note that this requires the user to lift-finger and re-try.
            //
            // Note that the inverse does work properly without needing anything extra.
            // That is, if you pan the webview to the top and then attempt to do a
            // pull-down gesture (slowly) the pulley menu will eventually (after interactive
            // gets reset back to "true" once the timer timeout expires) be activated,
            // without requiring the user to lift their finger and try again.
            // I wish that the other way around "just worked" also...
            flickTimer.start()
        }
    }

    Timer {
        id: flickTimer
        interval: 1000
        property bool showPullDownMenu: (webView == null || webView.atYBeginning) && flickable.pullDownMenu !== null
        property bool showPushUpMenu: (webView == null || webView.atYEnd) && flickable.pushUpMenu !== null
        onShowPullDownMenuChanged: flickable.pullDownMenu.enabled = showPullDownMenu
        onShowPushUpMenuChanged: flickable.pushUpMenu.enabled = showPushUpMenu

        property WebView webView
        function findChildWithProperty(item, propertyName) {
            if ('children' in item) {
                var children = item.children
                for (var i in children) {
                    var child = children[i]
                    if (child !== null && child !== undefined) {
                        if (child.hasOwnProperty(propertyName)) {
                            return child
                        } else {
                            var possible = findChildWithProperty(child, propertyName)
                            if (possible !== null) {
                                return possible
                            }
                        }
                    }
                }
            }
            return null
        }
        function findWebView() {
            if (flickTimer.webView === null) {
                flickTimer.webView = flickTimer.findChildWithProperty(flickable, "__sailfish_webview")
            }
        }
    }

    onChildrenChanged: flickTimer.findWebView()
    Component.onCompleted: flickTimer.findWebView()
}
