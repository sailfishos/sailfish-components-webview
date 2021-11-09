/****************************************************************************
**
** Copyright (c) 2016 - 2017 Jolla Ltd.
** Copyright (c) 2020 - 2021 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.2
import Sailfish.Silica 1.0
import Sailfish.WebView.Popups 1.0 as Popups
import Sailfish.WebView.Controls 1.0 as Controls

Timer {
    id: root

    property var pageStack
    property Item parentItem
    property QtObject tabModel: null
    property QtObject contentItem
    readonly property bool active: contextMenu && contextMenu.active || false
    property Item contextMenu

    property PopupProvider popupProvider: PopupProvider {}
    readonly property var _messageTopicToPopupProviderPropertyMapping: ({
        "Content:ContextMenu":  "contextMenu",
        "embed:alert":          "alertPopup",
        "embed:confirm":        "confirmPopup",
        "embed:prompt":         "promptPopup",
        "embed:login":          "passwordManagerPopup",
        "embed:auth":           "authPopup",
        "embed:permissions":  { "geolocation": "locationPermissionPopup" },
        "embed:webrtcrequest":  "webrtcPermissionPopup",
        "embed:popupblocked":   "blockedTabPopup",
        "embed:select":         "selectorPopup"
    })
    readonly property var listeners: Object.keys(_messageTopicToPopupProviderPropertyMapping)

    property var authDialogContentItem
    property var authDialogData
    property var authDialogWinId

    property bool downloadsEnabled: true

    property Component _contextMenuComponent
    property var _popupObject
    property var _delayedOpenValues

    property Notice positioningDisabledNotice: Notice {
        duration: 3000
        //% "Positioning is disabled"
        text: qsTrId("sailfish_components_webview_popupopener-la-positioning_disabled")
        anchor: Notice.Center
    }

    signal aboutToOpenPopup(var topic, var data)
    signal aboutToOpenContextMenu(var data)
    signal loginSaved

    function getCheckbox(data) {
        var inputs = data.inputs
        for (var i = 0; inputs && (i < inputs.length); ++i) {
            if (inputs[i].hint === "preventAddionalDialog") {
                return inputs[i]
            }
        }
        return null
    }

    function getCheckboxValue(checkbox) {
        if (checkbox) {
            // Check for undefined / null
            if (checkbox.checked !== null && checkbox.checked !== undefined) {
                return checkbox.checked
            } else if (checkbox.value !== null && checkbox.value !== undefined) {
                return checkbox.value
            }
        }
        return false
    }

    function getMenuList(data) {
        var inputs = data.inputs
        for (var i = 0; inputs && (i < inputs.length); ++i) {
            if (inputs[i].type === "menulist") {
                return inputs[i].values
            }
        }
        return null
    }

    function getButtonStringKeys(data, defaultButtons) {
        var buttons = data.buttons || defaultButtons;
        while (buttons.length < defaultButtons.length) {
            buttons.push("")
        }
        return buttons
    }

    // Returns true if message is handled.
    function message(topic, data) {
        if (!handlesMessage(topic)) {
            return false
        }

        if (!contentItem) {
            console.warn("Picker has no contentItem. Assign / Bind contentItem for each PickerOpener.")
            return false
        }

        if (!pageStack) {
            console.log("PopupOpener has no pageStack. Add missing binding.")
            return false
        }

        if (data.text && data.text.length > 1000) {
            console.log("Message is too big to show and will be truncated.")
            data.text  = data.text.substring(0, 1000) + " ..."
        }

        aboutToOpenPopup(topic, data)

        _delayedOpenValues = null
        _popupObject = null
        switch (topic) {
        case "Content:ContextMenu": root._openContextMenu(data); break;
        case "embed:alert":         alert(data);    break;
        case "embed:confirm":       confirm(data);  break;
        case "embed:prompt":        prompt(data);   break;
        case "embed:login":         login(data);    break;
        case "embed:auth":          auth(data);     break;
        case "embed:permissions": {
            if (data.title === "geolocation") {
                permissions(data)
            } else {
                // Currently we don't support other permission requests.
                sendAsyncMessage("embedui:permissions", {
                    "allow": false,
                    "checkedDontAsk": false,
                    "id": data.id
                })
            }
            break
        }
        case "embed:webrtcrequest": webrtc(data);   break;
        case "embed:popupblocked":  blocked(data);  break;
        case "embed:select":        selector(data); break;
        }
        // If we end up here, message has been handled.
        return true
    }

    // Open alert dialog
    function alert(data) {
        var checkbox = getCheckbox(data)
        var checkboxPrefill = getCheckboxValue(checkbox)
        var winId = data.winId
        var buttons = getButtonStringKeys(data, ["OK", ""])
        var props = {
            "text": data.text,
            "acceptText": buttons[0],
            "cancelText": buttons[1],
            "preventDialogsVisible": !(checkbox == null),
            "preventDialogsPrefillValue": checkboxPrefill
        }
        var acceptFn = function(popup) {
            _popupObject = null
            contentItem.sendAsyncMessage("alertresponse", {
                "winId": winId,
                "checkvalue": popup.preventDialogsValue
            })
        }
        var rejectFn = acceptFn

        openPopupByTopic("embed:alert", null, props, acceptFn, rejectFn)
    }

    // Open confirm dialog
    function confirm(data) {
        var checkbox = getCheckbox(data)
        var checkboxPrefill = getCheckboxValue(checkbox)
        var winId = data.winId
        var buttons = getButtonStringKeys(data, ["OK", "Cancel"])
        if (buttons.length > 2) {
            console.log("Requesting " + buttons.length + " buttons, but only two are supported.")
        }
        var props = {
            "text": data.text,
            "acceptText": buttons[0],
            "cancelText": buttons[1],
            "preventDialogsVisible": !(checkbox == null),
            "preventDialogsPrefillValue": checkboxPrefill
        }
        var acceptFn = function(popup) {
            _popupObject = null
            contentItem.sendAsyncMessage("confirmresponse", {
                "winId": winId,
                "accepted": true,
                "checkvalue": popup.preventDialogsValue
            })
        }
        var rejectFn = function(popup) {
            _popupObject = null
            contentItem.sendAsyncMessage("confirmresponse", {
                "winId": winId,
                "accepted": false,
                "checkvalue": popup.preventDialogsValue
            })
        }

        openPopupByTopic("embed:confirm", null, props, acceptFn, rejectFn)
    }

    // Open prompt dialog
    function prompt(data) {
        var defaultValue = !!data.defaultValue
                         ? data.defaultValue
                         : (!!data.inputs[0].value
                            ? data.inputs[0].value
                            : "")

        var checkbox = getCheckbox(data)
        var checkboxPrefill = getCheckboxValue(checkbox)
        var winId = data.winId
        var buttons = getButtonStringKeys(data, ["OK", "Cancel"])
        var props = {
            "text": data.text,
            "defaultValue": defaultValue,
            "acceptText": buttons[0],
            "cancelText": buttons[1],
            "preventDialogsVisible": !(checkbox == null),
            "preventDialogsPrefillValue": checkboxPrefill
        }
        var acceptFn = function(popup) {
            _popupObject = null
            contentItem.sendAsyncMessage("promptresponse", {
                "winId": winId,
                "accepted": true,
                "promptvalue": popup.value,
                "checkvalue": popup.preventDialogsValue
            })
        }
        var rejectFn = function(popup) {
            _popupObject = null
            contentItem.sendAsyncMessage("promptresponse", {
                "winId": winId,
                "accepted": false,
                "checkvalue": popup.preventDialogsValue
            })
        }

        openPopupByTopic("embed:prompt", null, props, acceptFn, rejectFn)
    }

    // Open login dialog
    function login(data) {
        var props = {
            "notificationType": data.name,
            "messageBundle": data.textBundle,
            "formData": data.formdata,
            "_internalData": { "contentItem": contentItem, "requestId": data.id }
        }
        var acceptFn = function(popup) {
            _popupObject = null
            popup._internalData.contentItem.sendAsyncMessage("embedui:login", {
                "buttonidx": 0, // "Yes" button
                "id": popup._internalData.requestId,
                "selectedIndex": popup.selectedIndex // Only if there are multiple logins
            })
            root.loginSaved()
        }
        var rejectFn = function(popup) {
            _popupObject = null
            popup._internalData.contentItem.sendAsyncMessage("embedui:login", {
                "buttonidx": 1, // "No" button
                "id": popup._internalData.requestId
            })
        }

        openPopupByTopic("embed:login", null, props, acceptFn, rejectFn)
    }

    // Open password manager dialog
    function auth(data) {
        var winId = data.winId
        var inputs = data.inputs
        var username
        var password
        var remember
        for (var i = 0; i < inputs.length; ++i) {
            if (inputs[i].hint === "username") {
                username = inputs[i]
            } else if (inputs[i].hint === "password") {
                password = inputs[i]
            } else if (inputs[i].hint === "remember") {
                remember = inputs[i]
            }
        }
        var passwordOnly = !username
        var buttons = getButtonStringKeys(data, ["AcceptLogin", ""])

        var props = {
            "messageBundle": data.text,
            "acceptText": buttons[0],
            "cancelText": buttons[1],
            "hostname": data.hostname || "",
            "realm": data.realm || "",
            "usernameVisible": !(username == null),
            "usernamePrefillValue": (!(username == null)) ? username.value : "",
            "usernameAutofocus": (!(username == null)) ? username.autofocus : false,
            "passwordPrefillValue": (!(password == null)) ? password.value : "",
            "rememberVisible": !(remember == null),
            "rememberPrefillValue": (!(remember == null)) ? remember.checked : false,
            "rememberMessageBundle": (!(remember == null)) ? remember.label : "",
            "passwordOnly": passwordOnly,
            "privateBrowsing": data.privateBrowsing || false
        }
        var acceptFn = function(popup) {
            _popupObject = null
            contentItem.sendAsyncMessage("authresponse", {
                "winId": winId,
                "accepted": true,
                "username": popup.usernameValue,
                "password": popup.passwordValue,
                "remember": popup.rememberValue
            })
        }
        var rejectFn = function(popup) {
            _popupObject = null
            contentItem.sendAsyncMessage("authresponse", {
                "winId": winId,
                "accepted": false
            })
        }

        openPopupByTopic("embed:auth", null, props, acceptFn, rejectFn)
    }

    // Open permissions dialog
    function permissions(data) {
        var props = {
            "host": data.host,
            "privateBrowsing": data.privateBrowsing
        }
        var acceptFn = function(popup) {
            _popupObject = null
            contentItem.sendAsyncMessage("embedui:permissions", {
                "allow": true,
                "checkedDontAsk": popup.rememberValue,
                "id": data.id
            })
            if (!Popups.LocationSettings.locationEnabled) {
                positioningDisabledNotice.show()
            }
        }
        var rejectFn = function(popup) {
            _popupObject = null
            contentItem.sendAsyncMessage("embedui:permissions", {
                "allow": false,
                "checkedDontAsk": popup.rememberValue,
                "id": data.id
            })
        }

        openPopupByTopic("embed:permissions", data.title, props, acceptFn, rejectFn)
    }

    function webrtc(data) {
        // Promote only supported requests and only from an observable origin
        if (data.origin && ("camera" in data.devices ||
                            "microphone" in data.devices)) {
            openPopupByTopic("embed:webrtcrequest", null, {
                    "origin": data.origin,
                    "devices": data.devices
                },
                // accept
                function(popup) {
                    contentItem.sendAsyncMessage("embedui:webrtcresponse", {
                        "allow": true,
                        "checkedDontAsk": popup.rememberValue,
                        "choices": popup.choices,
                        "id": data.id
                    })
                },
                // reject
                function(popup) {
                    contentItem.sendAsyncMessage("embedui:webrtcresponse", {
                        "allow": false,
                        "checkedDontAsk": popup.rememberValue,
                        "choices": popup.choices,
                        "id": data.id
                    })
                }
            )
        } else {
            contentItem.sendAsyncMessage("embedui:webrtcresponse", {
                "allow": false,
                "checkedDontAsk": false,
                "choices": {},
                "id": data.id
            })
        }
    }

    function blocked(data) {
        openPopupByTopic("embed:popupblocked", null,
            // properties
            { "host": data.host },
            // accept
            function(popup) {
                Controls.PermissionManager.add(
                        popup.host,
                        "popup",
                        Controls.PermissionManager.Allow,
                        popup.rememberValue // rule expiry
                            ? Controls.PermissionManager.Never
                            : Controls.PermissionManager.Session)
            },
            // reject
            function(popup) {
                if (popup.rememberValue) {
                    Controls.PermissionManager.add(
                            popup.host,
                            "popup",
                            Controls.PermissionManager.Deny,
                            Controls.PermissionManager.Never) // expiry
                }
            }
        )
    }

    function selector(data) {
        var winId = data.winId
        var buttons = getButtonStringKeys(data, ["OK", ""])
        var props = {
            "title": data.title,
            "text": data.text,
            "acceptText": buttons[0],
            "cancelText": buttons[1],
            "values": getMenuList(data)
        }

        var acceptFn = function(popup) {
            _popupObject = null
            contentItem.sendAsyncMessage("selectresponse", {
                "winId": winId,
                "button": 0,
                "menulist0": popup.selectedIndex
            })
        }
        var rejectFn = function(popup) {
            _popupObject = null
            contentItem.sendAsyncMessage("selectresponse", {
                "winId": winId,
                "button": 1
            })
        }

        openPopupByTopic("embed:select", data.title, props, acceptFn, rejectFn)
    }

    // Handle pagestack busy change
    function busyChanged() {
        if (!pageStack.busy && root._delayedOpenValues) {
            pageStack.busyChanged.disconnect(busyChanged)
            openPopup(root._delayedOpenValues[0],
                      root._delayedOpenValues[1],
                      root._delayedOpenValues[2],
                      root._delayedOpenValues[3],
                      root._delayedOpenValues[4])
            root._delayedOpenValues = null
        }
    }

    function handlesMessage(topic) {
        return listeners.indexOf(topic) >= 0
    }

    function openPopup(comp, properties, isDialog, acceptedFn, rejectedFn) {
        if (pageStack.busy) {
            _delayedOpenValues = [comp, properties, isDialog, acceptedFn, rejectedFn]
            pageStack.busyChanged.connect(busyChanged)
            return
        }

        var obj = null
        if (isDialog) {
            obj = pageStack.animatorPush(comp, properties)
            obj.pageCompleted.connect(function(dialog) {
                // TODO: also the Async message must be sent when window gets closed
                _popupObject = dialog // prevent gc()
                dialog.accepted.connect(function() { acceptedFn(dialog) })
                dialog.rejected.connect(function() { rejectedFn(dialog) })
            })
        } else {
            var component = comp.createObject === undefined
                          ? Qt.createComponent(comp) // TODO: asynchronous...
                          : comp
            if (!component) {
                console.log("PopupOpener.qml: unable to create component from", comp)
            } else if (component.status !== Component.Ready) {
                console.log("PopupOpener.qml: error loading component", comp)
                if (component.status === Component.Error) {
                    console.log("PopupOpener.qml: error:", component.errorString())
                } else if (component.status === Component.Null) {
                    console.log("PopupOpener.qml: error: component is null")
                }
            } else {
                obj = component.createObject(parentItem, properties)
                _popupObject = obj // prevent gc()
                obj.accepted.connect(function() { acceptedFn(obj) })
                obj.rejected.connect(function() { rejectedFn(obj) })
            }
        }
    }

    function openPopupByTopic(topic, subtopic, properties, acceptedFn, rejectedFn) {
        var comp = _resolveListenerComponent(topic, subtopic)
        var compIsDialog = _resolveListenerComponentType(topic, subtopic) === "dialog"
        if (comp === null || comp === undefined) {
            console.log("PopupOpener.qml: invalid component specified for: " + topic + " " + subtopic)
        } else {
            openPopup(comp, properties, compIsDialog, acceptedFn, rejectedFn)
        }
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
                contextMenu.tabModel = root.tabModel
                contextMenu.viewId = contentItem.uniqueId
                contextMenu.pageStack = root.pageStack
                contextMenu.show()
            } else {
                _contextMenuComponent = Qt.createComponent(_resolveListenerComponent("Content:ContextMenu"))
                if (_contextMenuComponent.status !== Component.Error) {
                    contextMenu = _contextMenuComponent.createObject(parentItem, {
                        "linkHref": linkHref,
                        "imageSrc": imageSrc,
                        "linkTitle": linkTitle && linkTitle.trim() || "",
                        "linkProtocol": data.linkProtocol,
                        "contentType": contentType,
                        "tabModel": root.tabModel,
                        "viewId": contentItem.uniqueId,
                        "pageStack": pageStack,
                        "downloadsEnabled": root.downloadsEnabled
                    })
                    contextMenu.show()
                } else {
                    console.log("Can't load ContextMenu component")
                }
            }
        }
    }

    Component.onCompleted: {
        // Warmup location settings.
        Popups.LocationSettings.locationEnabled
        Controls.PermissionManager.instance()
        if (contentItem) {
            for (var i = 0; i < listeners.length; ++i) {
                contentItem.addMessageListener(listeners[i])
            }
        } else {
            console.log("PopupOpener has no contentItem. Each created WebView/WebPage",
                        "instance can have own PopupOpener. Add missing binding.")
        }
    }

    function _resolveListenerComponent(topic, subtopic) {
        var comp = null
        var providerProperty = _messageTopicToPopupProviderPropertyMapping[topic]
        if (providerProperty === null || providerProperty === undefined) {
            console.log("No Popup registered for unknown message type: " + topic)
        } else if (subtopic !== null && subtopic !== undefined && subtopic.length > 0
                && providerProperty[subtopic] !== null && providerProperty[subtopic] !== undefined) {
            providerProperty = providerProperty[subtopic]
        }

        if (providerProperty !== null && providerProperty !== undefined) {
            var resolvedProperty = popupProvider[providerProperty]
            if (resolvedProperty !== null
                    && resolvedProperty !== undefined) {
                if (typeof resolvedProperty === 'string'
                        || resolvedProperty instanceof String) {
                    // specified as a component URL
                    comp = resolvedProperty
                } else if (!resolvedProperty.hasOwnProperty("component")) {
                    // specified as an inline component
                    comp = resolvedProperty
                } else {
                    // specified as a dictionary with type+component fields
                    comp = resolvedProperty["component"]
                }
            }

            // the default components are specified as URLs
            // so that they can be compiled lazily.
            if (typeof comp === 'string' || comp instanceof String) {
                comp = Qt.resolvedUrl(comp)
            }
        }

        return comp
    }

    function _resolveListenerComponentType(topic, subtopic) {
        var compType = null
        var providerProperty = _messageTopicToPopupProviderPropertyMapping[topic]
        if (providerProperty === null || providerProperty === undefined) {
            console.log("No Popup registered for unknown message type: " + topic)
        } else if (subtopic !== null && subtopic !== undefined && subtopic.length > 0
                && providerProperty[subtopic] !== null && providerProperty[subtopic] !== undefined) {
            providerProperty = providerProperty[subtopic]
        }

        if (providerProperty !== null && providerProperty !== undefined) {
            var resolvedProperty = popupProvider[providerProperty]
            if (resolvedProperty !== null
                    && resolvedProperty !== undefined) {
                if (typeof resolvedProperty === 'string'
                        || resolvedProperty instanceof String) {
                    // specified as a component URL, assume item-derived
                    compType = "item"
                } else if (!resolvedProperty.hasOwnProperty("component")) {
                    // specified as an inline component, assume item-derived
                    compType = "item"
                } else {
                    // specified as a dictionary with type+component fields
                    compType = resolvedProperty["type"]
                }
            }

            // if no value, assume normal Item type
            if (compType === undefined || compType === null) {
                compType = "item"
            }
        }

        return compType
    }
}
