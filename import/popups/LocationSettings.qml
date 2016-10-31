/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Raine Makelainen <raine.makelainen@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

pragma Singleton
import QtQml 2.2
import com.jolla.settings.system 1.0
import MeeGo.Connman 0.2

QtObject {
    id: root

    property alias enabled: locationSettings.locationEnabled
    property alias gpsPowered: gpsTechModel.powered

    property LocationSettings l: LocationSettings {
        id: locationSettings
    }

    property TechnologyModel t: TechnologyModel {
        id: gpsTechModel
        name: "gps"
    }
}
