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

    signal linkClicked(string url)

    active: true
    property WebViewPage webViewPage
    property int __sailfish_webview

    onActiveChanged: helper.setActiveInPage()
    Component.onCompleted: helper.setActiveInPage()
    onViewInitialized: {
        webview.loadFrameScript("chrome://embedlite/content/embedhelper.js");
        webview.loadFrameScript("chrome://embedlite/content/SelectAsyncHelper.js");
        webview.addMessageListeners([
            "embed:linkclicked",
            "embed:filepicker",
            "embed:selectasync",
            "embed:alert",
            "embed:confirm",
            "embed:prompt",
            "embed:auth",
            "embed:permissions",
            "embed:login",
            "Content:ContextMenu"]);
    }

    onRecvAsyncMessage: {
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
            case "embed:filepicker": {
                filePicker.createObject(pageStack, {
                                           "pageStack": pageStack,
                                           "winid": winid,
                                           "webView": webView,
                                           "filter": data.filter,
                                           "mode": data.mode})
                break
            }
            case "embed:selectasync": {
                pageStack.push(data.multiple ? multiSelectDialog : singleSelectPage,
                               { "options": data.options, "webview": webView });
                break
            }
            case "embed:alert": {
                dialog = pageStack.push(alertDialog, { "text": data.text })
                // TODO: also the Async message must be sent when window gets closed
                dialog.done.connect(function() {
                    webView.sendAsyncMessage("alertresponse", {"winid": winid})
                })
                break
            }
            case "embed:confirm": {
                dialog = pageStack.push(confirmDialog, { "text": data.text })
                // TODO: also the Async message must be sent when window gets closed
                dialog.accepted.connect(function() {
                    webView.sendAsyncMessage("confirmresponse",
                                             { "winid": winid, "accepted": true })
                })
                dialog.rejected.connect(function() {
                    webView.sendAsyncMessage("confirmresponse",
                                             { "winid": winid, "accepted": false })
                })
                break
            }
            case "embed:prompt": {
                dialog = pageStack.push(promptDialog, { "text": data.text, "value": data.defaultValue })
                // TODO: also the Async message must be sent when window gets closed
                dialog.accepted.connect(function() {
                    webView.sendAsyncMessage("promptresponse",
                                             { "winid": winid, "accepted": true, "promptvalue": dialog.value })
                })
                dialog.rejected.connect(function() {
                    webView.sendAsyncMessage("promptresponse",
                                             { "winid": winid, "accepted": false })
                })
                break
            }
            case "embed:auth": {
                helper.openAuthDialog(webView, data, winid)
                break
            }
            case "embed:permissions": {
                if (data.title === "geolocation"
                        && locationSettings.locationEnabled
                        && gpsTechModel.powered) {
                    switch (privData.geolocationUrls[data.host]) {
                        case "accepted": {
                            webView.sendAsyncMessage("embedui:premissions",
                                                     { "allow": true, "checkedDontAsk": false, "id": data.id })
                            break
                        }
                        case "rejected": {
                            webView.sendAsyncMessage("embedui:premissions",
                                                     { "allow": false, "checkedDontAsk": false, "id": data.id })
                            break
                        }
                        default: {
                            dialog = pageStack.push(locationDialog, {"host": data.host })
                            dialog.accepted.connect(function() {
                                webView.sendAsyncMessage("embedui:premissions",
                                                         { "allow": true, "checkedDontAsk": false, "id": data.id })
                                privData.geolocationUrls[data.host] = "accepted"
                            })
                            dialog.rejected.connect(function() {
                                webView.sendAsyncMessage("embedui:premissions",
                                                         { "allow": false, "checkedDontAsk": false, "id": data.id })
                                privData.geolocationUrls[data.host] = "rejected"
                            })
                            break
                        }
                    }
                } else {
                    // Currently we don't support other permission requests.
                    sendAsyncMessage("embedui:premissions",
                                     { "allow": false, "checkedDontAsk": false, "id": data.id })
                }
                break
            }
            case "embed:login": {
                dialog = pageStack.push(passwordManagerDialog,
                                        { "webView": webView, "requestId": data.id,
                                          "notificationType": data.name, "formData": data.formdata })
                break
            }
            case "Content:ContextMenu": {
                if (data.types.indexOf("image") !== -1 || data.types.indexOf("link") !== -1) {
                    var linkHref = data.linkURL
                    var imageSrc = data.mediaURL
                    var linkTitle = data.linkTitle
                    var contentType = data.contentType
                    if (privData.contextMenu) {
                        privData.contextMenu.linkHref = linkHref
                        privData.contextMenu.imageSrc = imageSrc
                        privData.contextMenu.linkTitle = linkTitle.trim()
                        privData.contextMenu.linkProtocol = data.linkProtocol || ""
                        privData.contextMenu.contentType = contentType
                        privData.contextMenu.viewId = webView.uniqueID()
                        privData.contextMenu.tabModel = null // TODO ??
                        // PageStack and WebView are currently always the same but
                        // let's update them regardless so that they will remain correct.
                        privData.contextMenu.pageStack = pageStack
                        privData.hideVirtualKeyboard()
                        privData.contextMenu.show()
                    } else {
                        privData.contextMenu = contextMenuComponent.createObject(webView,
                                { "linkHref": linkHref, "imageSrc": imageSrc,
                                  "linkTitle": linkTitle.trim(), "linkProtocol": data.linkProtocol,
                                  "contentType": contentType, "tabModel": null,
                                  "viewId": webView.uniqueID(),
                                  "pageStack": pageStack })
                        privData.hideVirtualKeyboard()
                        privData.contextMenu.show()
                    }
                }
                break
            }
            default: {
                console.log("unhandled async message received: " + message)
                break
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

    Timer {
        id: helper
        property var geolocationUrls: {{}}
        property var contextMenu

        function hideVirtualKeyboard() {
            if (Qt.inputMethod.visible) {
                webview.focus = true
            }
        }

        function openAuthDialog(webView, data, winid) {
            if (pageStack.busy) {
                helper.delayedOpenAuthDialog(webView, data, winid)
            } else {
                helper.immediateOpenAuthDialog(webView, data, winid)
            }
        }

        function immediateOpenAuthDialog(webView, data, winid) {
            var dialog = pageStack.push(authDialog,
                                        {"hostname": data.text, "realm": data.title,
                                         "username": data.storedUsername, "password": data.storedPassword,
                                         "passwordOnly": data.passwordOnly })
            dialog.accepted.connect(function () {
                webView.sendAsyncMessage("authresponse",
                                         { "winid": winid, "accepted": true,
                                           "username": dialog.username, "password": dialog.password,
                                           "dontsave": dialog.dontsave })
            })
            dialog.rejected.connect(function() {
                webView.sendAsyncMessage("authresponse",
                                         { "winid": winid, "accepted": false})
            })
        }

        property var authDialogWebView
        property var authDialogData
        property var authDialogWinId
        function delayedOpenAuthDialog(webView, data, winid) {
            authDialogWebView = webView
            authDialogData = data
            authDialogWinId = winid
            start()
        }

        repeat: false
        running: false
        interval: 600 // page transition delay.
        onTriggered: {
            openAuthDialog(authDialogWebView, authDialogData, authDialogWinId)
        }

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

        function setActiveInPage() {
            if (webview.active) {
                if (webview.webViewPage == null || webview.webViewPage == undefined) {
                    webview.webViewPage = helper.findParentWithProperty(webview, '__sailfish_webviewpage')
                }
                if (webview.webViewPage != null && webview.webViewPage != undefined) {
                    webview.webViewPage.activeWebView = webview
                }
            }
        }
    }

    SilicaPrivate.VirtualKeyboardObserver {
        id: virtualKeyboardObserver
        active: webview.enabled
        transpose: __silica_applicationwindow_instance._transpose
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

    Component { id: filePicker; PickerCreator {} }
    Component { id: multiSelectDialog; MultiSelectDialog {} }
    Component { id: singleSelectPage; SingleSelectPage {} }

    Component { id: authDialog; AuthDialog {} }
    Component { id: alertDialog; AlertDialog {} }
    Component { id: confirmDialog; ConfirmDialog {} }
    Component { id: locationDialog; LocationDialog {} }
    Component { id: passwordManagerDialog; PasswordManagerDialog {} }
    Component { id: promptDialog; PromptDialog {} }

    Component { id: contextMenuComponent; ContextMenu {} }
}
