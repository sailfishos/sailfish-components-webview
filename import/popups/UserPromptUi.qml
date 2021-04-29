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

Item {
    id: root

    // inputs
    property Dialog dialog
    property UserPromptInterface popupInterface

    default property alias defaultContent: promptContent.children

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: contentColumn.height

        Column {
            id: contentColumn
            width: parent.width

            DialogHeader {
                id: header
                dialog: dialog
                _glassOnly: true
                acceptText: popupInterface.acceptText
                cancelText: popupInterface.cancelText
                title: popupInterface.title
            }

            Item {
                id: promptContent
                width: dialog.width

                // So that anchoring works in user prompt dialog implementations
                // and dialog looks visually good. This leaves annoying binding loop which happens
                // when keyboard is opened as keyboard opening reduces dialog height.
                height: Math.max(childrenRect.height, dialog.height - header.height - preventDialogs.height) - Theme.paddingLarge * 2
            }

            TextSwitch {
                id: preventDialogs

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
