/*
 * SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
 * SPDX-License-Identifier: MPL-2.0
 */
import QtQuick 2.6
import Sailfish.Silica 1.0

Item {
    id: handler

    property var webView
    property var pageStack
    property var requests: ({})
    property var prompt

    function current(request) {
        if (!webView || !request || Date.now() >= request.expiresAt) return false
        var tabs = webView.tabModel.snapshot()
        for (var i = 0; i < tabs.length; ++i) {
            if (String(tabs[i].tabId) === request.tabId
                    && String(tabs[i].locationRevision) === request.locationRevision) return true
        }
        return false
    }

    function respond(request, outcome, remember) {
        if (requests[request.id] !== request) return
        delete requests[request.id]
        if (webView) {
            webView.sendAsyncMessageToTab(request.tabId, "externalurlresponse", {
                "winId": request.winId, "id": request.id, "outcome": outcome,
                "remember": outcome === "dispatched" && request.canRemember && remember === true
            })
        }
        if (Object.keys(requests).length === 0) expiryTimer.stop()
    }

    function dispatch(request, remember) {
        if (requests[request.id] !== request) return
        if (!current(request)) {
            respond(request, "cancelled")
            return
        }
        // Use the platform URL dispatcher: mailto/tel/custom schemes belong to
        // their registered applications, not necessarily the browser.
        respond(request, Qt.openUrlExternally(request.url) ? "dispatched" : "unsupported", remember)
    }

    function receive(tabId, data) {
        var id = String(data.id)
        if (requests[id]) return
        // QtMoz supplies the source tab and its navigation revision. Never
        // answer through the selected tab, which may have changed meanwhile.
        var request = {
            "id": id, "winId": data.winId, "tabId": String(tabId),
            "locationRevision": String(data.locationRevision), "url": String(data.url),
            "host": data.host || "", "expiresAt": Date.now() + 110000,
            "canRemember": data.canRemember === true && !data.privateBrowsing
        }
        for (var pendingId in requests) {
            if (requests[pendingId].tabId === request.tabId) respond(requests[pendingId], "cancelled")
        }
        requests[id] = request
        var scheme = /^([a-z][a-z0-9+.-]*):/i.exec(request.url)
        scheme = scheme ? scheme[1].toLowerCase() : ""
        if (!scheme || ["http", "https", "file", "data", "javascript", "about", "chrome", "resource"].indexOf(scheme) !== -1) {
            respond(request, "unsupported")
        } else if (!current(request)) {
            respond(request, "cancelled")
        } else {
            // An intent may select different applications for the same scheme.
            if (scheme === "intent") request.canRemember = false
            if (data.permissionAllowed === true && scheme !== "intent") {
                dispatch(request, false)
            } else if (!pageStack) {
                respond(request, "cancelled")
            } else {
                expiryTimer.start()
            }
        }
        nextPrompt()
    }

    function nextPrompt() {
        if (!pageStack || pageStack.busy) return
        if (prompt) {
            if (requests[prompt.request.id] !== prompt.request && prompt.status === PageStatus.Active) prompt.reject()
            return
        }
        for (var id in requests) {
            var request = requests[id]
            if (!current(request)) {
                respond(request, "cancelled")
                continue
            }
            prompt = pageStack.push(promptComponent, {"request": request})
            if (!prompt) respond(request, "cancelled")
            return
        }
    }

    function discardStaleRequests() {
        for (var id in requests) {
            if (!current(requests[id])) respond(requests[id], "cancelled")
        }
        nextPrompt()
    }

    Connections {
        target: handler.webView
        onRecvAsyncMessageFromTab: {
            if (message === "embed:externalurl") handler.receive(tabId, data)
        }
    }
    Connections {
        target: handler.webView ? handler.webView.tabModel : null
        onRevisionChanged: handler.discardStaleRequests()
    }
    Connections {
        target: handler.pageStack
        onBusyChanged: handler.nextPrompt()
    }
    Timer {
        id: expiryTimer

        interval: 1000
        repeat: true
        onTriggered: handler.discardStaleRequests()
    }
    Component {
        id: promptComponent

        Dialog {
            id: dialog

            property var request

            onAccepted: handler.dispatch(request, rememberPermission.checked)
            onRejected: handler.respond(request, "declined")
            onStatusChanged: {
                if (status === PageStatus.Inactive && handler.prompt === dialog) {
                    handler.respond(request, "cancelled")
                    handler.prompt = null
                    handler.nextPrompt()
                }
            }

            SilicaFlickable {
                anchors.fill: parent
                contentHeight: column.height

                Column {
                    id: column

                    width: parent.width
                    DialogHeader {
                        //% "Open external application?"
                        title: qsTrId("sailfish_components_webview-he-external_application")
                        //% "Open"
                        acceptText: qsTrId("sailfish_components_webview-he-open_external_application")
                    }
                    Label {
                        x: Theme.horizontalPageMargin
                        width: parent.width - 2 * x
                        wrapMode: Text.Wrap
                        textFormat: Text.PlainText
                        //% "%1 wants to open %2 in another application."
                        text: qsTrId("sailfish_components_webview-la-open_external_application").arg(request.host).arg(request.url)
                    }
                    TextSwitch {
                        id: rememberPermission

                        visible: request.canRemember
                        //% "Always allow this site to open this application"
                        text: qsTrId("sailfish_components_webview-la-remember_external_application")
                    }
                }
            }
        }
    }
    Component.onCompleted: webView.addMessageListener("embed:externalurl")
    Component.onDestruction: {
        for (var id in requests) respond(requests[id], "cancelled")
    }
}
