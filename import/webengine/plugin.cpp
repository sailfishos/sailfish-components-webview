/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Raine Makelainen <raine.makelainen@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "webengine.h"
#include "webenginesettings.h"
#include "downloadhelper.h"

#include <QtCore/QStandardPaths>
#include <QQmlExtensionPlugin>
#include <qqml.h>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

template <typename T> static QObject *singletonApiFactory(QQmlEngine *engine, QJSEngine *)
{
    return new T(engine);
}

class SailfishOSWebEnginePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Sailfish.WebEngine" )

public:
    void initializeEngine(QQmlEngine *, const char *)
    {
        // TODO : How to deal with custom default UA. We have also means to customize
        // site specific UA overrides.
        SailfishOS::WebEngine::initialize(QString());
        SailfishOS::WebEngineSettings::initialize();
    }

    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("Sailfish.WebEngine"));

        qmlRegisterSingletonType<SailfishOS::WebEngine>("Sailfish.WebEngine", 1, 0, "WebEngine",
                                                        singletonApiFactory<SailfishOS::WebEngine>);
        qmlRegisterSingletonType<SailfishOS::WebEngineSettings>("Sailfish.WebEngine", 1, 0, "WebEngineSettings",
                                                                singletonApiFactory<SailfishOS::WebEngineSettings>);
        qmlRegisterSingletonType<SailfishOS::WebEngineUtils::DownloadHelper>("Sailfish.WebEngine", 1, 0,
                                                                             "DownloadHelper",
                                                                             singletonApiFactory<SailfishOS::WebEngineUtils::DownloadHelper>);
    }
};

#include "plugin.moc"
