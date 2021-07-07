/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

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
                clip: flickable.contentHeight > height

                SilicaFlickable {
                    id: flickable
                    anchors.fill: parent
                    contentHeight: content.height + Theme.paddingLarge

                    Column {
                        id: content
                        width: parent.width - 2 * Theme.horizontalPageMargin
                        anchors.centerIn: parent
                        spacing: Theme.paddingLarge

                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                            font.bold: true
                            text: popup.title
                        }
                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                            text: "Allow " + popup.host + " to see your location?"
                        }
                        TextSwitch {
                            id: remember
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "Remember for this site"
                        }
                        TextSwitch {
                            id: toggle
                            anchors.horizontalCenter: parent.horizontalCenter
                            height: visible ? implicitHeight : 0
                            visible: popup.preventDialogsVisible
                            checked: popup.preventDialogsVisible && popup.preventDialogsPrefillValue
                            text: "Don't show this again"
                        }
                        Button {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: popup.acceptText
                            onClicked: { popup.accepted(); popup.visible = false }
                        }
                        Button {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: popup.cancelText
                            onClicked: { popup.rejected(); popup.visible = false }
                        }
                    }
                }
            }
        }
    }
}
