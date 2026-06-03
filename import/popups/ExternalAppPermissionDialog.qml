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

Dialog {
    id: dialog

    property alias privateBrowsing: externalApp.privateBrowsing
    property alias host: externalApp.host
    property alias scheme: externalApp.scheme
    property alias targetUrl: externalApp.targetUrl
    property alias rememberValue: externalApp.rememberValue

    property alias acceptText: externalApp.acceptText
    property alias cancelText: externalApp.cancelText
    property alias title: externalApp.title

    ExternalAppPermissionInterface {
        id: externalApp

        anchors.fill: parent
        rememberValue: remember.checked

        //: Accept an external application launch request
        //% "Open"
        acceptText: qsTrId("sailfish_components_webview_popups-he-open_external_app")

        //: Deny an external application launch request
        //% "Cancel"
        cancelText: qsTrId("sailfish_components_webview_popups-he-cancel_external_app")

        //% "Open link in another app?"
        title: qsTrId("sailfish_components_webview_popups-ti-open_external_app")

        UserPromptUi {
            anchors.fill: parent
            dialog: dialog
            popupInterface: externalApp

            Column {
                width: parent.width
                spacing: Theme.paddingMedium

                Label {
                    x: Theme.horizontalPageMargin
                    width: parent.width - 2 * x
                    wrapMode: Text.WordWrap
                    color: Theme.highlightColor
                    text: externalApp.requestText()
                }

                Label {
                    x: Theme.horizontalPageMargin
                    width: parent.width - 2 * x
                    visible: text.length > 0
                    wrapMode: Text.WrapAnywhere
                    color: Theme.secondaryHighlightColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                    textFormat: Text.PlainText
                    text: externalApp.targetUrl
                }

                TextSwitch {
                    id: remember

                    visible: !externalApp.privateBrowsing
                    //: Remember external application launch permission for the site
                    //% "Always allow for this site"
                    text: qsTrId("sailfish_components_webview_popups-remember_external_app_for_site")
                }

                Label {
                    x: Theme.horizontalPageMargin
                    width: parent.width - 2 * x
                    visible: externalApp.privateBrowsing
                    wrapMode: Text.Wrap
                    color: Theme.highlightColor

                    //: Description label for user when private mode active (entered permissions are not saved)
                    //% "When in private browsing mode permissions choices are not stored"
                    text: qsTrId("sailfish_components_webview_popups-la-private_browsing_premission_description")
                }
            }
        }

        function requestText() {
            if (externalApp.host) {
                //: %1 is the site and %2 is the URI scheme that will be opened by another application
                //% "Allow %1 to open %2 links in another app?"
                return qsTrId("sailfish_components_webview_popups-la-open_external_app_request")
                        .arg(externalApp.host).arg(externalApp.scheme)
            }

            //: %1 is the URI scheme that will be opened by another application
            //% "Allow this site to open %1 links in another app?"
            return qsTrId("sailfish_components_webview_popups-la-open_external_app_request_without_host")
                    .arg(externalApp.scheme)
        }
    }
}
