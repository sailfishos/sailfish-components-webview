/****************************************************************************
**
** Copyright (c) 2016 - 2020 Jolla Ltd.
** Copyright (c) 2020 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.6
import Sailfish.Silica 1.0
import Sailfish.WebView.Controls 1.0

Row {
    id: root

    property TextSelectionController controller

    property bool isPhoneNumber: controller && controller.isPhoneNumber
    property bool hasSearch: controller && controller.searchUri !== ""
    property bool selectAllEnabled

    property real _buttonWidth: width / (repeater.count + 1)

    readonly property var defaultButtons: {
        var buttons = []

        if (isPhoneNumber) {
            buttons.push({
                "icon": "image://theme/icon-m-answer",
                "action": function () {
                    clear()
                    call()
                }
            })
        }
        buttons.push({
            "icon": "image://theme/icon-m-share",
            "action": function () {
                clear()
                share()
            }
        })

        if (selectAllEnabled) {
            buttons.push({ "icon": "image://theme/icon-m-select-all", "action": selectAll })
        }

        if (hasSearch) {
            buttons.push({
                "icon": "image://theme/icon-m-search",
                "action": function () {
                    clear()
                    search()
                }
            })
        }
        buttons.push({
            "icon": "image://theme/icon-m-input-remove",
            "action": clear
        })

        return buttons
    }

    property var buttons: defaultButtons

    signal call
    signal search
    signal share

    function selectAll() {
        if (controller) {
            controller.selectAll()
        }
    }

    function clear() {
        if (controller) {
            controller.clearSelection()
        }
    }

    height: Theme.itemSizeSmall

    Repeater {
        id: repeater

        model: root.buttons

        IconButton {
            width: (root.width - root.leftPadding - root.rightPadding) / Math.max(1, repeater.count)
            height: root.height
            icon.source: modelData.icon
            onClicked: root.buttons[index].action()
        }
    }
}
