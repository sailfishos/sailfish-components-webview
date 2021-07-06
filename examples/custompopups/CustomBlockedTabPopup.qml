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
                    text: "Site " + popup.host + " wants to open a popup.  Allow?"
                }
                TextSwitch {
                    id: toggle
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Remember decision for this site"
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
