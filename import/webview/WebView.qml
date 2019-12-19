/****************************************************************************
**
** Copyright (c) 2016 - 2020 Jolla Ltd.
** Copyright (c) 2019 - 2020 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Silica.private 1.0 as SilicaPrivate
import Sailfish.WebView 1.0
import Sailfish.WebView.Controls 1.0
import Sailfish.WebView.Popups 1.0
import Sailfish.WebView.Pickers 1.0

RawWebView {
    id: webview

    property PageStack webViewPageStack
    property WebViewPage webViewPage
    property int __sailfish_webview
    property bool canShowSelectionMarkers: true

    readonly property bool textSelectionActive: textSelectionController && textSelectionController.active
    property Item textSelectionController: null

    signal linkClicked(string url)

    function _findParentWithProperty(item, propertyName) {
        var parentItem = item.parent
        while (parentItem) {
            if (parentItem.hasOwnProperty(propertyName)) {
                return parentItem
            }
            parentItem = parentItem.parent
        }
        return null
    }

    function _hasWebViewPage() {
        return (webview.webViewPage != null && webview.webViewPage != undefined)
    }

    function _hasWebViewPageStack() {
        return (webview.webViewPageStack != null && webview.webViewPageStack != undefined)
    }

    function clearSelection() {
        if (textSelectionActive) {
            textSelectionController.clearSelection()
        }
    }

    function _setActiveInPage() {
        if (webview.active) {
            if (!_hasWebViewPage()) {
                webview.webViewPage = webview._findParentWithProperty(webview, '__sailfish_webviewpage')
            }

            if (_hasWebViewPage()) {
                webview.webViewPage.activeWebView = webview
            }
        }

        if (!_hasWebViewPage()) {
            console.warn("WebView.qml it is mandatory to declare webViewPage property to get orientation change working correctly!")
        }

        if (!_hasWebViewPageStack()) {
            webview.webViewPageStack = webview._findParentWithProperty(webview, "_pageStackIndicator")
        }
    }

    active: true
    _acceptTouchEvents: !textSelectionActive

    onActiveChanged: webview._setActiveInPage()
    Component.onCompleted: webview._setActiveInPage()
    onParentChanged: webview._setActiveInPage()

    onViewInitialized: {
        webview.loadFrameScript("chrome://embedlite/content/embedhelper.js");
        webview.addMessageListeners([
                                        "embed:linkclicked",
                                        "Content:ContextMenu",
                                        "Content:SelectionRange",
                                        "Content:SelectionCopied",
                                        "Content:SelectionSwap"
                                    ]);
    }

    onRecvAsyncMessage: {
        if (pickerOpener.message(message, data) || popupOpener.message(message, data)) {
            return
        }

        switch(message) {
            case "embed:linkclicked": {
                webview.linkClicked(data.uri)
                break
            }
            case "Content:SelectionRange": {
                if (textSelectionController === null) {
                    textSelectionController = textSelectionControllerComponent.createObject(
                                webview, {"contentItem" : webview})
                }
                textSelectionController.selectionRangeUpdated(data)
                break
            }
            case "Content:SelectionSwap": {
                if (textSelectionController) {
                    textSelectionController.swap()
                }

                break
            }
            default: {
                break
            }
        }
    }
    onRecvSyncMessage: {
        // sender expects that this handler will update `response` argument
        switch (message) {
        case "Content:SelectionCopied": {
            if (data.succeeded && textSelectionController) {
                textSelectionController.showNotification()
            }
            response.message = {"": ""}
            break
        }
        }
    }

    Component {
        id: textSelectionControllerComponent

        TextSelectionController {
            opacity: canShowSelectionMarkers ? 1.0 : 0.0
            contentWidth: webview.webViewPage ? webview.webViewPage.width : webview.width
            contentHeight: Math.max(webview.contentHeight+webview.bottomMargin+webview.topMargin, webview.height)
            anchors {
                fill: parent
            }

            Behavior on opacity { FadeAnimator {} }
        }
    }

    PickerOpener {
        id: pickerOpener

        property QtObject pageStackOwner: webview._findParentWithProperty(webview, "pageStack")

        pageStack: pageStackOwner ? pageStackOwner.pageStack : undefined
        contentItem: webview
    }

    PopupOpener {
        id: popupOpener

        pageStack: pickerOpener.pageStack
        parentItem: webview
        contentItem: webview

        onAboutToOpenContextMenu: {
            if (Qt.inputMethod.visible) {
                webview.parent.focus = true
            }

            if (data.types.indexOf("content-text") !== -1) {
                // we want to select some content text
                webview.sendAsyncMessage("Browser:SelectionStart", {"xPos": data.xPos, "yPos": data.yPos})
            }
        }
    }

    BusyIndicator {
        id: busySpinner
        anchors.centerIn: parent
        running: true
        visible: webview.loading
        size: BusyIndicatorSize.Large
    }

    SilicaPrivate.VirtualKeyboardObserver {
        id: virtualKeyboardObserver

        readonly property QtObject appWindow: webview._findParentWithProperty(webview, "__silica_applicationwindow_instance")

        active: webview.enabled
        transpose: appWindow ? appWindow._transpose : false
        onImSizeChanged: {
            if (imSize > 0 && opened) {
                webview.virtualKeyboardMargin = virtualKeyboardObserver.imSize
            }
        }

        orientation: {
            if (webview.webViewPage != null) {
                return webview.webViewPage.orientation
            } else if (webViewPageStack != undefined && webViewPageStack != null) {
                if (webViewPageStack.currentPage !== undefined && webViewPageStack.currentPage !== null) {
                    return webViewPageStack.currentPage.orientation
                } else {
                    return Orientation.Portrait
                }
            } else {
                return Orientation.Portrait
            }
        }

        onOpenedChanged: {
            if (opened) {
                webview.virtualKeyboardMargin = virtualKeyboardObserver.panelSize
            }
        }
        onClosedChanged: {
            if (closed) {
                webview.virtualKeyboardMargin = 0
            }
        }
    }
}
