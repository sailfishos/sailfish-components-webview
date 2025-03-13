/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.WebView.Popups 1.0

PasswordManagerPopupInterface {
    id: popup

    width: (parent.width / 5) * 4
    height: (parent.height / 5) * 4
    anchors.centerIn: parent

    property string displayUser: popup.formData["displayUser"]
    property string displayHost: popup.formData["displayHost"]
    property string text: popup.notificationType == "password-save"
                        ? (displayUser.length ? ("Save password for user " + displayUser + " on " + displayHost + "?")
                                              : ("Save password on " + displayHost + "?"))
                        : popup.notificationType == "password-change"
                          ? (displayUser.length ? ("Update password for user " + displayUser + "?")
                                                : ("Update password?"))
                          : "Unknown password manager request!"

    Rectangle {
        anchors.fill: parent
        color: "lightgrey"
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
                    text: popup.text
                }
                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Save password"
                    onClicked: {
                        popup.accepted()
                        popup.visible = false
                    }
                }
                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Cancel"
                    onClicked: {
                        popup.rejected()
                        popup.visible = false
                    }
                }
            }
        }
    }
}
