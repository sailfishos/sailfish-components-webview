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

Dialog {
    id: dialog

    property alias text: confirm.text

    property alias acceptText: confirm.acceptText
    property alias cancelText: confirm.cancelText
    property alias title: confirm.title
    property alias preventDialogsVisible: confirm.preventDialogsVisible
    property alias preventDialogsPrefillValue: confirm.preventDialogsPrefillValue
    property alias preventDialogsValue: confirm.preventDialogsValue

    ConfirmPopupInterface {
        id: confirm

        anchors.fill: parent

        //: Text on the Accept dialog button that accepts browser's confirm() messages
        //% "Ok"
        acceptText: qsTrId("sailfish_components_webview_popups-he-accept_confirm")

        UserPromptUi {
            anchors.fill: parent
            dialog: dialog
            popupInterface: confirm

            PromptLabel {
                text: confirm.text
            }
        }
    }
}
