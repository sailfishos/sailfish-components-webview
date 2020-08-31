/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Copyright (c) 2020 Open Mobile Platform LLC.
** Contact: Raine Makelainen <raine.makelaine@jolla.com>
** Contact: Chris Adams <chris.adams@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "downloadhelper.h"

#include <QFileInfo>

SailfishOS::WebEngineUtils::DownloadHelper::DownloadHelper(QObject *parent)
    : QObject(parent)
{
}

QString SailfishOS::WebEngineUtils::DownloadHelper::createUniqueFileUrl(QString fileName, const QString &path) const
{
    if (path.isEmpty()) {
        return QString();
    }

    // Remove illegal characters from filename
    fileName.replace(QRegExp("[<>:\"|?*%/\\\\]+"), "");

    // Remove trailing dots and whitespaces
    fileName.replace(QRegExp("[\\.\\s_]+$"), "");

    // Remove leading whitespaces
    fileName.replace(QRegExp("^[\\s_]+"), "");

    // Replace whitespace characters with underscores
    fileName.replace(QRegExp("[\\s_]+"), "_");

    if (fileName.isEmpty()) {
        fileName = QStringLiteral("unnamed_file");
    }

    const QFileInfo fileInfo(fileName);
    const QString newFile("%1/%2(%3)%4%5");
    const QString baseName = fileInfo.baseName();
    const QString suffix = fileInfo.completeSuffix();

    QString result(path + "/" + fileName);
    int collisionCount(1);

    while (QFileInfo::exists(result)) {
        collisionCount++;
        result = newFile.arg(path).arg(baseName).arg(collisionCount).arg(suffix.isEmpty() ? "" : ".").arg(suffix);
    }

    return result;
}
