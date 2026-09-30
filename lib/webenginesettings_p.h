/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAILFISHOS_WEBENGINE_SETTINGS_P_H
#define SAILFISHOS_WEBENGINE_SETTINGS_P_H

#include <QObject>
#include <QString>
#include <webenginesettings.h>

#ifndef Q_QDOC

namespace SailfishOS {

class WebEngineSettingsPrivate : public QObject
{
    Q_OBJECT

public:
    static WebEngineSettingsPrivate * instance();

    explicit WebEngineSettingsPrivate(QObject *parent = 0);
    ~WebEngineSettingsPrivate();

public slots:
    void notifyColorSchemeChanged();
    void oneShotNotifyColorSchemeChanged(const QString &message, const QVariant &data);

    friend class WebEngineSettings;
};

}

#endif // !Q_QDOC
#endif // SAILFISHOS_WEBENGINE_SETTINGS_P_H
