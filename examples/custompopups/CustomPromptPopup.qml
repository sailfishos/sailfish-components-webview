/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.WebView.Popups 1.0

PromptPopupInterface {
    id: popup

    property bool canAccept: input.text.length > 0

    width: (parent.width/5)*4
    height: (parent.height/5)*4
    anchors.centerIn: parent

    acceptText: "Ok"
    cancelText: "Cancel"

    preventDialogsValue: toggle.checked

    onAccepted: value = input.text

    Rectangle {
        anchors.fill: parent
        color: "lightcoral"

        Text {
            id: title
            anchors.bottom: input.top
            anchors.bottomMargin: Theme.paddingLarge
            width: parent.width
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            text: popup.title
        }
        TextField {
            id: input
            anchors.centerIn: parent
            width: parent.width
            focus: true
            label: text.length > 0 ? popup.text : ""
            placeholderText: popup.text
            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
            EnterKey.enabled: text.length > 0
            EnterKey.iconSource: "image://theme/icon-m-enter-accept"
            EnterKey.onClicked: { if (popup.canAccept) { popup.accepted(); popup.visible = false } }
        }
        TextSwitch {
            id: toggle
            anchors.horizontalCenter: input.horizontalCenter
            anchors.top: input.bottom
            height: visible ? implicitHeight : 0
            visible: popup.preventDialogsVisible
            checked: popup.preventDialogsVisible && popup.preventDialogsPrefillValue
            text: "Don't show this again"
        }
        Button {
            id: acceptBtn
            anchors.horizontalCenter: input.horizontalCenter
            anchors.top: toggle.bottom
            anchors.topMargin: Theme.paddingLarge
            enabled: popup.canAccept
            text: popup.acceptText
            onClicked: { popup.accepted(); popup.visible = false }
        }
        Button {
            id: rejectBtn
            anchors.horizontalCenter: input.horizontalCenter
            anchors.top: acceptBtn.bottom
            anchors.topMargin: Theme.paddingLarge
            text: popup.cancelText
            onClicked: { popup.rejected(); popup.visible = false }
        }
    }
}
