/****************************************************************************
**
** Copyright (c) 2026 Jolla Mobile Ltd
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.6
import Sailfish.Silica 1.0

QtObject {
    id: controller

    property var webPage
    property bool _frameScriptLoaded

    readonly property real textZoom: Math.pow(Theme.fontSizeMedium / Theme.fontSizeMediumBase, 1.25)

    property Connections webPageConnections: Connections {
        target: controller.webPage
        onDomContentLoadedChanged: {
            if (controller.webPage.domContentLoaded) {
                controller.updateTextZoom()
            }
        }
    }

    function updateTextZoom() {
        if (!webPage) {
            return
        }

        if (!_frameScriptLoaded) {
            webPage.loadFrameScript(Qt.resolvedUrl("TextZoom.js"))
            _frameScriptLoaded = true
        }

        if (webPage.domContentLoaded) {
            webPage.sendAsyncMessage("embedui:textZoom", { "zoom": textZoom })
        }
    }

    onWebPageChanged: {
        _frameScriptLoaded = false
        updateTextZoom()
    }

    onTextZoomChanged: updateTextZoom()

    Component.onCompleted: updateTextZoom()
}
