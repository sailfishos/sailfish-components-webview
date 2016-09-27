import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.WebView 1.0

SilicaFlickable {
    id: flickable

    interactive: (flickable.pullDownMenu !== null && flickable.pullDownMenu.active)
              || (flickable.pushUpMenu !== null && flickable.pushUpMenu.active)
              || (helper.showPullDownMenu || helper.showPushUpMenu)

    QtObject {
        id: helper
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
            if (helper.webView === null) {
                helper.webView = helper.findChildWithProperty(flickable, "__sailfish_webview")
                if (helper.webView) {
                    helper.webView.flickable = flickable
                }
            }
        }
    }

    onChildrenChanged: helper.findWebView()
    Component.onCompleted: helper.findWebView()
}
