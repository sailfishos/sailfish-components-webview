/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Raine Makelainen <raine.makelaine@jolla.com>
** Contact: Chris Adams <chris.adams@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAILFISHOS_WEBENGINE_DOWNLOADHELPER_H
#define SAILFISHOS_WEBENGINE_DOWNLOADHELPER_H

#include <QObject>

namespace SailfishOS {

namespace WebEngineUtils {

class DownloadHelper : public QObject
{
    Q_OBJECT
public:
    DownloadHelper(QObject *parent = Q_NULLPTR);
    Q_INVOKABLE QString createUniqueFileUrl(const QString &fileName, const QString &path) const;
};

}
}

#endif // SAILFISHOS_WEBENGINE_DOWNLOADHELPER_H
