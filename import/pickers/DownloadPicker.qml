/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.0
import Sailfish.WebEngine 1.0
import Sailfish.Pickers 1.0

FolderPickerPage {
    property var data

    headerSpacing: 0
    showSystemFiles: false

    //% "Download to"
    dialogTitle: qsTrId("sailfish_browser-ti-download-to")

    onSelectedPathChanged: {
        WebEngine.notifyObservers("embedui:downloadpicker",
                                     {
                                         "downloadDirectory": selectedPath,
                                         "defaultFileName": data.defaultFileName,
                                         "suggestedFileExtension": data.suggestedFileExtension
                                     })
    }
}
