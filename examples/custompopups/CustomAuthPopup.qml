/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.WebView.Popups 1.0

AuthPopupInterface {
    id: popup
    width: (parent.width/5)*4
    height: (parent.height/5)*4
    anchors.centerIn: parent

    usernameValue: username.text
    passwordValue: password.text
    rememberValue: remember.checked

    acceptText: "Login"
    cancelText: "Cancel"

    Rectangle {
        anchors.fill: parent
        color: "cyan"
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
                    text: "" + popup.hostname + " requires authentication: " + popup.realm
                }

                TextField {
                    id: username

                    width: parent.width
                    focus: popup.usernameVisible && popup.usernameAutofocus
                    visible: !popup.passwordOnly && popup.usernameVisible
                    placeholderText: "Enter your user name"
                    label: "User name"

                    text: popup.usernamePrefillValue
                    inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
                    EnterKey.iconSource: "image://theme/icon-m-enter-next"
                    EnterKey.onClicked: password.focus = true
                }

                PasswordField {
                    id: password

                    width: parent.width
                    placeholderText: "Enter password"
                    text: popup.passwordPrefillValue

                    EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                    EnterKey.onClicked: { popup.accepted(); popup.visible = false }
                }

                TextSwitch {
                    id: remember

                    visible: !popup.privateBrowsing && !(popup.rememberVisible && popup.rememberPrefillValue)
                    checked: popup.rememberPrefillValue // bind the output value for when the prefill value is true.
                    text: "Remember these details?"
                }

                Button {
                    id: acceptBtn
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: popup.acceptText
                    onClicked: { popup.accepted(); popup.visible = false }
                }

                Button {
                    id: rejectBtn
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: popup.cancelText
                    onClicked: { popup.rejected(); popup.visible = false }
                }
            }
        }
    }
}
