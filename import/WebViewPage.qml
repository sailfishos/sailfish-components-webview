import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: webViewPage

    property var activeWebView

    property int __sailfish_webviewpage

    states: [
        State {
            name: "webViewActive"
            when: activeWebView != null && activeWebView != undefined && activeWebView.visible && activeWebView.active
            PropertyChanges {
                target: pageStack
                _noGrabbing: activeWebView.moving || activeWebView.pinching || activeWebView.dragging
            }
            PropertyChanges {
                target: webViewPage
                backNavigation: activeWebView.atXBeginning && !activeWebView.pinching
                forwardNavigation: webViewPage._belowTop && activeWebView.atXEnd && !activeWebView.pinching
            }
        }
    ]
}
