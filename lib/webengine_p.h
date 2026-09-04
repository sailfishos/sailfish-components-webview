/****************************************************************************
**
** Copyright (C) 2026 Jolla Mobile Ltd
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAILFISHOS_WEBENGINE_P_H
#define SAILFISHOS_WEBENGINE_P_H

#include <QStringList>

class QMozContext;

namespace SailfishOS {

class WebEnginePrivate
{
public:
    WebEnginePrivate();

    QMozContext *context;
    QStringList userStyleSheets;
};

}

#endif // SAILFISHOS_WEBENGINE_P_H
