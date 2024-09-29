/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Raine Makelainen <raine.makelaine@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAILFISHOS_WEBENGINE_SETTINGS_H
#define SAILFISHOS_WEBENGINE_SETTINGS_H

#include <QObject>
#include <QString>
#include <qmozenginesettings.h>

#ifndef Q_QDOC

namespace SailfishOS {

class WebEngineSettingsPrivate;

class WebEngineSettings : public QMozEngineSettings
{
    Q_OBJECT

public:
    static void initialize();
    static WebEngineSettings *instance();

    explicit WebEngineSettings(QObject *parent = 0);
    virtual ~WebEngineSettings();

private:
    WebEngineSettingsPrivate *d;
};

}

#endif // !Q_QDOC
#endif // SAILFISHOS_WEBENGINE_SETTINGS_H
