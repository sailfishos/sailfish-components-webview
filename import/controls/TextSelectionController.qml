/****************************************************************************
**
** Copyright (c) 2013 - 2019 Jolla Ltd.
** Copyright (c) 2019 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.1
import Sailfish.Silica 1.0
import MeeGo.QOfono 0.2

MouseArea {
    id: root

    propagateComposedEvents: true

    property bool selectionVisible
    readonly property bool active: selectionVisible

    property alias contentWidth: start.contentWidth
    property alias contentHeight: start.contentHeight

    property alias contentItem: start.contentItem

    property string text
    property string searchUri
    property bool isPhoneNumber

    readonly property bool _canCall: cellular1Status.registered || cellular2Status.registered

    // keep selection range we get from engine to move the draggers with
    // selection together when panning or zooming
    property var _cssRange
    property var _selectionData

    property bool _phoneNumberSelected

    property alias startHandleMask: start.mask
    property alias endHandleMask: end.mask

    function selectionRangeUpdated(data) {
        var resolution = contentItem.resolution
        start.lineHeight = data.start.height * resolution
        end.lineHeight = data.end.height * resolution

        var startHeightShift = data.start.height / 2
        var endHeightShift = data.end.height / 2

        // Don't update root state yet.
        var state = data.src

        var visualViewport = data.visualViewport

        // Start marker
        start.fixedX = (data.start.xPos - visualViewport.offsetLeft) * resolution - start.width
        start.fixedY = (data.start.yPos - visualViewport.offsetTop) * resolution
        if (!selectionVisible) {
            start.x = start.fixedX
            start.y = start.fixedY
        } else if ((state === "end"  || state === "reflow") && !start.dragActive) {
            start.targetPositionAnimation.start()
        }

        // End marker
        end.fixedX = (data.end.xPos - visualViewport.offsetLeft) * resolution
        end.fixedY = (data.end.yPos - visualViewport.offsetTop) * resolution

        if (!selectionVisible) {
            end.x = end.fixedX
            end.y = end.fixedY
        }
        else if ((state === "end" || state === "reflow") && !end.dragActive) {
            end.targetPositionAnimation.start()
        }

        _cssRange = {
            "startX": data.start.xPos,
            "startY": data.start.yPos,
            "endX": data.end.xPos,
            "endY": data.end.yPos,
            "startHeightShift": startHeightShift,
            "endHeightShift": endHeightShift,
            "origOffsetX": contentItem.scrollableOffset.x,
            "origOffsetY": contentItem.scrollableOffset.y,
            "origResolution": resolution,
            "visualViewport": visualViewport
        }

        _selectionData = data
        selectionVisible = true

        text = data.text || ""
        searchUri = data.searchUri || ""

        _phoneNumberSelected = data.isPhoneNumber
        isPhoneNumber = _canCall && _phoneNumberSelected

        root.state = state
    }

    function copy() {
        if (selectionVisible) {
            // Send a message that hits the selection.
            contentItem.sendAsyncMessage("Browser:SelectionCopy",
                                         {
                                             "xPos": _cssRange.startX + 1,
                                             "yPos": _cssRange.startY - 1,
                                         })
        }
    }

    function selectAll() {
        contentItem.sendAsyncMessage("Browser:SelectionSelectAll", { })
    }

    function showNotification() {
        if (!notification.published) {
            notification.show()
            notification.published = true
        }
    }

    function swap() {
        // Should we implement this?
        // Feels rather good this way as well.
    }

    function clearSelection() {
        selectionVisible = false
        _cssRange = null
        contentItem.sendAsyncMessage("Browser:SelectionClose",
                                 {
                                     "clearSelection": true
                                 })

        notification.published = false
        root.destroy()
    }


    function getMarkerBaseMessage(markerTag) {
        var resolution = contentItem.resolution
        var offsetLeft = _cssRange.visualViewport.offsetLeft * resolution
        var offsetTop = _cssRange.visualViewport.offsetTop * resolution
        return {
            change: markerTag,
            start: {
                xPos: (start.x + offsetLeft + start.width) / resolution,
                yPos: (start.y + offsetTop)/ resolution
            },
            end: {
                xPos: (end.x + offsetLeft) / resolution,
                yPos: (end.y + offsetTop) / resolution
            },
            caret: {
                xPos: 0,
                yPos: 0
            },
        }
    }

    // Selection is copied upon state change.
    onPressed: {
        clearSelection()
        mouse.accepted = false
    }

    onStateChanged: {
        // Copy when selection starts and ends.
        if (state === "end" || state === "start") {
            copy()
        }
    }

    on_CanCallChanged: {
        isPhoneNumber = _canCall && _phoneNumberSelected
    }

    TextSelectionHandle {
        id: start
        markerTag: "start"

        // contentItem, contentWidth, and contentHeight are aliased
        // from root
        visible: selectionVisible
        selectionController: root
    }

    TextSelectionHandle {
        id: end

        markerTag: "end"
        contentItem: root.contentItem
        contentWidth: root.contentWidth
        contentHeight: root.contentHeight
        visible: selectionVisible
        selectionController: root
    }

    Notice {
        id: notification
        property bool published

        duration: 3000

        //% "Copied to clipboard"
        text: qsTrId("sailfish_components_webview_textselection-la-selection_copied")

        verticalOffset: -Theme.itemSizeMedium
    }

    OfonoManager {
        id: ofonoManager
    }

    OfonoNetworkRegistration {
        id: cellular1Status

        property bool registered: status == "registered" || status == "roaming"

        modemPath: ofonoManager.modems[0] || ""
    }

    OfonoNetworkRegistration {
        id: cellular2Status

        property bool registered: status == "registered" || status == "roaming"

        modemPath: ofonoManager.modems[1] || ""
    }
}
