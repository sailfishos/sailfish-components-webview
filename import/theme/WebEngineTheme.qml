// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
// SPDX-License-Identifier: MPL-2.0

import QtQuick 2.6
import Sailfish.Silica 1.0

QtObject {
    readonly property bool darkTheme: Theme.colorScheme === Theme.LightOnDark

    onDarkThemeChanged: webEngineThemeSettings.setDarkTheme(darkTheme)
    Component.onCompleted: {
        webEngineThemeSettings.setThemePixelRatio(Theme.pixelRatio)
        webEngineThemeSettings.setDarkTheme(darkTheme)
    }
}
