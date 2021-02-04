/****************************************************************************
**
** Copyright (C) 2015
** Contact: Siteshwar Vashisht <siteshwar@gmail.com>
** Copyright (c) 2020 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */


#include <QObject>
#include <QStringList>
#include <QMap>

namespace SailfishOS {

class OpenSearchConfigs : public QObject {

public:
    static QMap<QString, QString> getAvailableOpenSearchConfigs();
    static QStringList getSearchEngineList();

private:
    QStringList m_openSearchPathList;

    inline static OpenSearchConfigs* getInstance();
    static OpenSearchConfigs *openSearchConfigs;
    inline QMap<QString, QString> parseOpenSearchConfigs();
    inline QStringList parseSearchEngineList();
    inline OpenSearchConfigs(QObject* parent=0);
};

}
