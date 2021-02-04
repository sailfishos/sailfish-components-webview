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


#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include "opensearchconfigs.h"

SailfishOS::OpenSearchConfigs *SailfishOS::OpenSearchConfigs::openSearchConfigs = 0;

SailfishOS::OpenSearchConfigs::OpenSearchConfigs(QObject *parent)
    : QObject(parent)
{
    m_openSearchPathList << QString(EMBEDLITE_CONTENT_PATH);
    m_openSearchPathList << QDir::homePath() + USER_OPENSEARCH_PATH;
}

QMap<QString, QString> SailfishOS::OpenSearchConfigs::parseOpenSearchConfigs()
{
    QMap<QString, QString> configs;

    foreach(QString openSearchPath, m_openSearchPathList) {
        QDir configDir(openSearchPath);
        configDir.setSorting(QDir::Name);

        foreach (QString fileName, configDir.entryList(QStringList("*.xml"))) {
            QFile xmlFile(openSearchPath + fileName);
            xmlFile.open(QIODevice::ReadOnly | QIODevice::Text);
            QXmlStreamReader xml(&xmlFile);
            QString searchEngine;

            while (!xml.atEnd()) {
                xml.readNext();
                if (xml.isStartElement() && xml.name() == "ShortName") {
                    xml.readNext();
                    if (xml.isCharacters()) {
                        searchEngine = xml.text().toString();
                    }
                }
            }

            if (!xml.hasError()) {
                configs.insert(searchEngine, openSearchPath + fileName);
            }

            xmlFile.close();
        }
    }
    return configs;
}

SailfishOS::OpenSearchConfigs* SailfishOS::OpenSearchConfigs::getInstance()
{
    if (!openSearchConfigs) {
        openSearchConfigs = new OpenSearchConfigs();
    }
    return openSearchConfigs;
}

QStringList SailfishOS::OpenSearchConfigs::getSearchEngineList()
{
    // Return names of search engines
    return getInstance()->parseOpenSearchConfigs().keys();
}

QMap<QString, QString> SailfishOS::OpenSearchConfigs::getAvailableOpenSearchConfigs()
{
    return getInstance()->parseOpenSearchConfigs();
}
