/****************************************************************************
**
** Copyright (c) 2013 - 2020 Jolla Ltd.
** Copyright (c) 2021 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.0
import Sailfish.Silica 1.0
import "StringUtils.js" as StringUtils

Item {
    id: root

    // inputs
    property Dialog dialog
    property UserPromptInterface popupInterface

    default property alias defaultContent: promptContent.children

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: contentContainer.height

        Item {
            id: contentContainer

            width: parent.width
            height: header.height + promptContent.height + preventDialogs.height

            DialogHeader {
                id: header

                dialog: dialog
                _glassOnly: true
                acceptText: StringUtils.geckoKeyToString(popupInterface.acceptText)
                cancelText: StringUtils.geckoKeyToString(popupInterface.cancelText)
                title: popupInterface.title
            }

            Column {
                id: promptContent

                anchors.top: header.bottom
                width: dialog.width
            }

            TextSwitch {
                id: preventDialogs

                anchors.top: promptContent.bottom
                height: visible ? implicitHeight : 0
                visible: popupInterface.preventDialogsVisible
                checked: popupInterface.preventDialogsVisible && popupInterface.preventDialogsPrefillValue
                onCheckedChanged: popupInterface.preventDialogsValue = checked

                //% "Prevent this page to create additional dialogs"
                text: qsTrId("sailfish_components_webview_popups-prevent_additional_dialogs")
            }
        }
    }
}
