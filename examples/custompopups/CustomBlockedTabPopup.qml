/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.WebView.Popups 1.0

BlockedTabPopupInterface {
    id: popup
    width: (parent.width/5)*4
    height: (parent.height/5)*4
    anchors.centerIn: parent

    rememberValue: toggle.checked

    property string acceptText: "Allow"
    property string cancelText: "Deny"

    Rectangle {
        anchors.fill: parent
        color: "chartreuse"

        Text {
            id: message
            anchors.centerIn: parent
            width: parent.width
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: "Site " + popup.host + " wants to open a popup.  Allow?"
        }
        TextSwitch {
            id: toggle
            anchors.horizontalCenter: message.horizontalCenter
            anchors.top: message.bottom
            anchors.topMargin: Theme.paddingLarge
            text: "Remember decision for this site"
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
