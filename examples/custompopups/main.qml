import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.WebView 1.0
import Sailfish.WebView.Popups 1.0

ApplicationWindow {
    id: root

    initialPage: Component {
        Page {
            id: page
            WebView {
                anchors.fill: parent
                active: true
                url: "https://browser.sailfishos.org/tests/testpage.html"
                popupProvider: PopupProvider {
                    id: customPopupProvider

                    // popup implementations may be provided via component URL string
                    passwordManagerPopup: "qrc:/CustomPasswordManagerPopup.qml"
                    authPopup: "qrc:/CustomAuthPopup.qml"

                    // or as Component instances
                    promptPopup: Qt.createComponent("qrc:/CustomPromptPopup.qml")
                    locationPermissionPopup: customLocationPermissionPopup

                    // or by dictionary with "type" and "component" fields
                    confirmPopup: ({"type": "item", "component": Qt.createComponent("qrc:/CustomConfirmPopup.qml")})
                    alertPopup: ({"type": "item", "component": "qrc:/CustomAlertPopup.qml"})
                    blockedTabPopup: ({"type": "item", "component": "qrc:/CustomBlockedTabPopup.qml"})

                    // if a custom implementation is not provided for some popup type,
                    // the default implementation will be used by the webview.
                    //contextMenu: "qrc:/CustomContextMenu.qml"
                }
            }
        }
    }

    Component {
        id: customLocationPermissionPopup
        LocationPopupInterface {
            id: popup
            width: (parent.width/5)*4
            height: (parent.height/5)*4
            anchors.centerIn: parent

            acceptText: "Allow"
            cancelText: "Disallow"
            preventDialogsValue: toggle.checked
            rememberValue: remember.checked

            Rectangle {
                anchors.fill: parent
                color: "blanchedalmond"

                Text {
                    id: title
                    anchors.bottom: message.top
                    anchors.bottomMargin: Theme.paddingLarge
                    width: parent.width
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    font.bold: true
                    text: popup.title
                }
                Text {
                    id: message
                    anchors.centerIn: parent
                    width: parent.width
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    text: "Allow " + popup.host + " to see your location?"
                }
                TextSwitch {
                    id: remember
                    anchors.horizontalCenter: message.horizontalCenter
                    anchors.top: message.bottom
                    anchors.topMargin: Theme.paddingLarge
                    text: "Remember for this site"
                }
                TextSwitch {
                    id: toggle
                    anchors.horizontalCenter: message.horizontalCenter
                    anchors.top: remember.bottom
                    anchors.topMargin: Theme.paddingLarge
                    height: visible ? implicitHeight : 0
                    visible: popup.preventDialogsVisible
                    checked: popup.preventDialogsVisible && popup.preventDialogsPrefillValue
                    text: "Don't show this again"
                }
                Button {
                    id: acceptBtn
                    anchors.horizontalCenter: message.horizontalCenter
                    anchors.top: toggle.bottom
                    anchors.topMargin: Theme.paddingLarge
                    text: popup.acceptText
                    onClicked: { popup.accepted(); popup.visible = false }
                }
                Button {
                    id: rejectBtn
                    anchors.horizontalCenter: message.horizontalCenter
                    anchors.top: acceptBtn.bottom
                    anchors.topMargin: Theme.paddingLarge
                    text: popup.cancelText
                    onClicked: { popup.rejected(); popup.visible = false }
                }
            }
        }
    }
}
