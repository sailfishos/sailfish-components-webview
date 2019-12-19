/****************************************************************************
**
** Copyright (c) 2016 - 2020 Jolla Ltd.
** Copyright (c) 2020 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Silica.private 1.0

Page {
    id: webViewPage

    property QtObject activeWebView

    property int __sailfish_webviewpage

    orientationTransitions: PageOrientationTransition {
        targetPage: webViewPage
        orientationChangeActions: [
            ScriptAction {
                script: {
                    if (activeWebView
                            && activeWebView.virtualKeyboardMargin > 0) {
                        activeWebView.updateContentSize(Qt.size(
                                    activeWebView.width,
                                    (activeWebView.virtualKeyboardMargin + activeWebView.height)))
                    }
                }
            },
            PauseAnimation {
                id: orientationChangeWait
                duration: 500
            }
        ]
    }

    Connections {
        target: activeWebView
        ignoreUnknownSignals: true
        onContentOrientationChanged: orientationChangeWait.complete()
    }
}
