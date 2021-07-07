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
    width: (parent.width/5)*4
    height: (parent.height/5)*4
    anchors.centerIn: parent

    acceptText: "Ok"
    cancelText: "Cancel"

    value: input.text
    preventDialogsValue: toggle.checked

    property bool canAccept: input.text.length > 0

    Rectangle {
        anchors.fill: parent
        color: "lightcoral"
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
                TextField {
                    id: input
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
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: visible ? implicitHeight : 0
                    visible: popup.preventDialogsVisible
                    checked: popup.preventDialogsVisible && popup.preventDialogsPrefillValue
                    text: "Don't show this again"
                }
                Button {
                    id: acceptBtn
                    anchors.horizontalCenter: parent.horizontalCenter
                    enabled: popup.canAccept
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
