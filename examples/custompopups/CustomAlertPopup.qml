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

    width: (parent.width / 5) * 4
    height: (parent.height / 5) * 4
    anchors.centerIn: parent

    acceptText: "Dismiss"
    preventDialogsValue: toggle.checked

    Rectangle {
        anchors.fill: parent
        color: "lightsteelblue"
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
                    text: popup.text
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
            }
        }
    }
}
