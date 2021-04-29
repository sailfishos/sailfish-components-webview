/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.WebView.Popups 1.0

AlertPopupInterface {
    id: popup
    width: (parent.width/5)*4
    height: (parent.height/5)*4
    anchors.centerIn: parent

    acceptText: "Dismiss"
    preventDialogsValue: toggle.checked

    Rectangle {
        anchors.fill: parent
        color: "lightsteelblue"

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
            text: popup.text
        }
        TextSwitch {
            id: toggle
            anchors.horizontalCenter: message.horizontalCenter
            anchors.top: message.bottom
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
    }
}
