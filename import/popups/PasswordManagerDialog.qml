/****************************************************************************
**
** Copyright (c) 2013 - 2020 Jolla Ltd.
** Copyright (c) 2021 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    id: dialog

    property alias notificationType: passwordManager.notificationType
    property alias formData: passwordManager.formData
    property alias _internalData: passwordManager._internalData

    PasswordManagerPopupInterface {
        id: passwordManager

        anchors.fill: parent

        Item {
            anchors.fill: parent

            DialogHeader {
                //: Accept browser's request to save entered password
                //% "Save"
                acceptText: qsTrId("sailfish_components_webview_popups-he-accept_password_mgr_request")
                _glassOnly: true
            }

            Label {
                id: label

                anchors.centerIn: parent
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                font {
                    family: Theme.fontFamilyHeading
                    pixelSize: Theme.fontSizeExtraLarge
                }
                color: Theme.highlightColor
                opacity: Theme.opacityHigh

                text: {
                    switch (passwordManager.notificationType) {
                        case "password-save": {
                            if (passwordManager.formData["displayUser"]) {
                                //% "Would you like to save password for user %1 on %2?"
                                return qsTrId("sailfish_components_webview_popups-la-save_password")
                                        .arg(passwordManager.formData["displayUser"])
                                        .arg(passwordManager.formData["displayHost"])
                            } else {
                                //% "Would you like to save password on %1?"
                                return qsTrId("sailfish_components_webview_popups-ls-save_password_no_user")
                                        .arg(passwordManager.formData["displayHost"])
                            }
                        }
                        case "password-change": {
                            if (passwordManager.formData["displayUser"]) {
                                //% "Would you like to update password for user %1?"
                                return qsTrId("sailfish_components_webview_popups-la-update_password")
                                        .arg(passwordManager.formData["displayUser"])
                            } else {
                                //% "Would you like to update password?"
                                return qsTrId("sailfish_components_webview_popups-la-update_password_no_user")
                            }
                        }
                        case "password-update-multiuser": {
                            // TODO: currently embedlite component for login manager promter heavily relies
                            //       on gecko's localization service for UI strings.
                            //       See LoginManagerPrompter.promtToChangePasswordWithUsernames() for details.
                            //       We need to reimplement it in order to use Qt l10n for password updates where
                            //       we don't know which existing login is being updated.
                            //       Though this task is quite a corner case and thus of very low priority.
                            console.log("TODO: password-update-multiuser notification type hasn't been implemented yet")
                            break
                        }
                        default: {
                            console.log("Unhandled password manager notification type: "
                                        + passwordManager.notificationType)
                            break
                        }
                    }
                }
            }
        }
    }
}
