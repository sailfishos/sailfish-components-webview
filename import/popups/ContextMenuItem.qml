/****************************************************************************
**
** Copyright (c) 2023 Jolla Ltd.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.WebView.Popups 1.0

ListItem {
    property alias text: menuItem.text

    width: parent.width
    height: screen.sizeCategory <= Screen.Medium ? Theme.itemSizeExtraSmall : Theme.itemSizeSmall

    MenuItem {
        id: menuItem
    }
}
