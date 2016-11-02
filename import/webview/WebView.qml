/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Chris Adams <chris.adams@jollamobile.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Silica.private 1.0 as SilicaPrivate
import Sailfish.WebView 1.0
import Sailfish.WebView.Popups 1.0
import Sailfish.WebView.Pickers 1.0

RawWebView {
    id: webview

    property WebViewPage webViewPage
    property int __sailfish_webview

    signal linkClicked(string url)

    active: true
    onActiveChanged: helper.setActiveInPage()
    Component.onCompleted: helper.setActiveInPage()
    onViewInitialized: {
        webview.loadFrameScript("chrome://embedlite/content/embedhelper.js");
        webview.loadFrameScript("chrome://embedlite/content/SelectAsyncHelper.js");
        webview.addMessageListeners([
                                        "embed:linkclicked",
                                    ]);
    }

    onRecvAsyncMessage: {
        if (pickerOpener.handlesMessage(message) || popupOpener.handlesMessage(message)) {
            return
        }

        // cache some symbol resolutions in var properties in the function closure
        var privData = helper
        var webView = webview
        var winid = data.winid
        var dialog = null
        switch(message) {
            case "embed:linkclicked": {
                webView.linkClicked(data.uri)
                break
            }
            default: {
                break
            }
        }
    }

    PickerOpener {
        id: pickerOpener

        property QtObject pageStackOwner: helper.findParentWithProperty(webView, "pageStack")

        pageStack: pageStackOwner ? pageStackOwner.pageStack : undefined
        contentItem: webview
    }

    PopupOpener {
        id: popupOpener

        pageStack: pickerOpener.pageStack
        parentItem: webview
        contentItem: webview
    }

    BusyIndicator {
        id: busySpinner
        anchors.centerIn: parent
        running: true
        visible: webview.loading
        size: BusyIndicatorSize.Large
    }

    Timer {
        id: helper

        function findParentWithProperty(item, propertyName) {
            var parentItem = item.parent
            while (parentItem) {
                if (parentItem.hasOwnProperty(propertyName)) {
                    return parentItem
                }
                parentItem = parentItem.parent
            }
            return null
        }

        function hasWebViewPage() {
            return (webview.webViewPage != null && webview.webViewPage != undefined)
        }

        function setActiveInPage() {
            if (webview.active) {
                if (!hasWebViewPage()) {
                    webview.webViewPage = helper.findParentWithProperty(webview, '__sailfish_webviewpage')
                }

                if (hasWebViewPage()) {
                    webview.webViewPage.activeWebView = webview
                }
            }

            if (!hasWebViewPage()) {
                console.warn("WebView.qml it is mandatory to declare webViewPage property to get orientation change working correctly!")
            }
        }
    }

    SilicaPrivate.VirtualKeyboardObserver {
        id: virtualKeyboardObserver

        readonly property QtObject appWindow: helper.findParentWithProperty(webview, "__silica_applicationwindow_instance")

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
            } else if (pageStack != undefined && pageStack != null) {
                if (pageStack.currentPage !== undefined && pageStack.currentPage !== null) {
                    return pageStack.currentPage.orientation
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
