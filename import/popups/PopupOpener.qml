/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Raine Makelainen <raine.makelainen@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.2
import Sailfish.WebView.Popups 1.0 as Popups

Timer {
    id: root

    property var geolocationUrls: {{}}
    property var pageStack
    property Item parentItem
    property var tabModel
    property QtObject contentItem
    readonly property bool active: contextMenu && contextMenu.active || false
    property Item contextMenu

    readonly property var listeners: ["embed:alert", "embed:confirm", "embed:prompt",
        "embed:login", "embed:auth", "embed:permissions", "Content:ContextMenu"]

    property var authDialogContentItem
    property var authDialogData
    property var authDialogWinId

    property Component _contextMenuComponent

    signal aboutToOpenContextMenu(var data)

    function hideVirtualKeyboard() {
        if (Qt.inputMethod.visible) {
            contentItem.focus = true
        }
    }

    function openAuthDialog(contentItem, data, winid) {
        if (pageStack.busy) {
            root._delayedOpenAuthDialog(contentItem, data, winid)
        } else {
            root._immediateOpenAuthDialog(contentItem, data, winid)
        }
    }

    function _delayedOpenAuthDialog(contentItem, data, winid) {
        authDialogContentItem = contentItem
        authDialogData = data
        authDialogWinId = winid
        start()
    }

    function _immediateOpenAuthDialog(contentItem, data, winid) {
        var dialog = pageStack.push(Qt.resolvedUrl("AuthDialog.qml"),
                                    {"hostname": data.text, "realm": data.title,
                                     "username": data.storedUsername, "password": data.storedPassword,
                                     "passwordOnly": data.passwordOnly })
        dialog.accepted.connect(function () {
            contentItem.sendAsyncMessage("authresponse",
                                     { "winid": winid, "accepted": true,
                                       "username": dialog.username, "password": dialog.password,
                                       "dontsave": dialog.dontsave })
        })
        dialog.rejected.connect(function() {
            contentItem.sendAsyncMessage("authresponse",
                                     { "winid": winid, "accepted": false})
        })
    }

    function handlesMessage(message) {
        return listeners.indexOf(message) >= 0
    }

    function _openContextMenu(data) {
        root.aboutToOpenContextMenu(data)
        if (data.types.indexOf("image") !== -1 || data.types.indexOf("link") !== -1) {
            var linkHref = data.linkURL
            var imageSrc = data.mediaURL
            var linkTitle = data.linkTitle
            var contentType = data.contentType

            if (contextMenu) {
                contextMenu.linkHref = linkHref
                contextMenu.imageSrc = imageSrc
                contextMenu.linkTitle = linkTitle.trim()
                contextMenu.linkProtocol = data.linkProtocol || ""
                contextMenu.contentType = contentType
                contextMenu.tabModel = tabModel
                contextMenu.viewId = contentItem.uniqueID()
                contextMenu.pageStack = root.pageStack
                hideVirtualKeyboard()
                contextMenu.show()
            } else {
                _contextMenuComponent = Qt.createComponent(Qt.resolvedUrl("ContextMenu.qml"))
                if (_contextMenuComponent.status !== Component.Error) {
                    contextMenu = _contextMenuComponent.createObject(parentItem,
                                                            {
                                                                "linkHref": linkHref,
                                                                "imageSrc": imageSrc,
                                                                "linkTitle": linkTitle.trim(),
                                                                "linkProtocol": data.linkProtocol,
                                                                "contentType": contentType,
                                                                "tabModel": tabModel,
                                                                "viewId": contentItem.uniqueID(),
                                                                "pageStack": pageStack
                                                            })
                    hideVirtualKeyboard()
                    contextMenu.show()
                } else {
                    console.log("Can't load ContextMenu.qml")
                }
            }
        }
    }

    repeat: false
    running: false
    interval: 600 // page transition delay.
    onTriggered: openAuthDialog(authDialogContentItem, authDialogData, authDialogWinId)

    property var _messageHandler: Connections {
        target: contentItem

        onRecvAsyncMessage: {
            if (!handlesMessage(message)) {
                return
            }

            if (!contentItem) {
                console.warn("Picker has no contentItem. Assign / Bind contentItem for each PickerOpener.")
                return
            }

            if (!pageStack) {
                console.log("PopupOpener has no pageStack. Add missing binding.")
                return
            }

            var winid = data.winid
            var dialog
            switch (message) {
            case "embed:alert": {
                dialog = pageStack.push(Qt.resolvedUrl("AlertDialog.qml"), { "text": data.text })
                // TODO: also the Async message must be sent when window gets closed
                dialog.done.connect(function() {
                    contentItem.sendAsyncMessage("alertresponse", {"winid": winid})
                })
                break
            }
            case "embed:confirm": {
                dialog = pageStack.push(Qt.resolvedUrl("ConfirmDialog.qml"), { "text": data.text })
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
                dialog = pageStack.push(Qt.resolvedUrl("PromptDialog.qml"), { "text": data.text, "value": data.defaultValue })
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
            case "embed:login": {
                dialog = pageStack.push(Qt.resolvedUrl("PasswordManagerDialog.qml"),
                                        { "webView": contentItem, "requestId": data.id,
                                          "notificationType": data.name, "formData": data.formdata })
                break
            }
            case "embed:auth": {
                root.openAuthDialog(contentItem, data, winid)
                break
            }
            case "embed:permissions": {
                if (data.title === "geolocation"
                        && Popups.LocationSettings.enabled
                        && Popups.LocationSettings.gpsPowered) {
                    switch (root.geolocationUrls[data.host]) {
                        case "accepted": {
                            contentItem.sendAsyncMessage("embedui:premissions",
                                                     { "allow": true, "checkedDontAsk": false, "id": data.id })
                            break
                        }
                        case "rejected": {
                            contentItem.sendAsyncMessage("embedui:premissions",
                                                     { "allow": false, "checkedDontAsk": false, "id": data.id })
                            break
                        }
                        default: {
                            dialog = pageStack.push(Qt.resolvedUrl("LocationDialog.qml"), {"host": data.host })
                            dialog.accepted.connect(function() {
                                contentItem.sendAsyncMessage("embedui:premissions",
                                                         { "allow": true, "checkedDontAsk": false, "id": data.id })
                                root.geolocationUrls[data.host] = "accepted"
                            })
                            dialog.rejected.connect(function() {
                                contentItem.sendAsyncMessage("embedui:premissions",
                                                         { "allow": false, "checkedDontAsk": false, "id": data.id })
                                root.geolocationUrls[data.host] = "rejected"
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
            case "Content:ContextMenu": {
                root._openContextMenu(data)
                break
            }
            }
        }
    }

    Component.onCompleted: {
        // Warmup location settings.
        Popups.LocationSettings.enabled
        if (contentItem) {
            contentItem.addMessageListeners(listeners)
        } else {
            console.log("PopupOpener has no contentItem. Each created WebView/WebPage",
                        "instance can have own PopupOpener. Add missing binding.")
        }
    }
}
