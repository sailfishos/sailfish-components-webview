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
constexpr int FILEEXTENSION_MAX_LENGTH = 32;
constexpr int FILENAME_MAX_LENGTH = 255;

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

    // Determine start position of file extension
    int dotPosition = fileName.length() < FILEEXTENSION_MAX_LENGTH ? 0 : fileName.length() - FILEEXTENSION_MAX_LENGTH;
    while (dotPosition < fileName.length() && fileName[dotPosition] != '.') {
        dotPosition += 1;
    }

    const QString baseName = fileName.left(dotPosition);
    const QString extension = fileName.mid(dotPosition);


    QString result;
    QString suffix = extension;
    int collisionCount = 1;

    do {
        result = QStringLiteral("%1/%2%4").arg(path).arg(baseName.left(FILENAME_MAX_LENGTH - suffix.length())).arg(suffix);
        collisionCount++;
        suffix = QStringLiteral("(%3)%4").arg(collisionCount).arg(extension);
    } while (QFileInfo::exists(result));

    return result;
}
