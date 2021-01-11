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

static QByteArray cutUtf8String(const QByteArray &str, int maxSize) {
    int size = str.size();
    if (str.size() > maxSize) {
        size = 0;
        for (int i = maxSize; i > 0; i--) {
            if ((str[i] & 0xC0) != 0x80) {
                size = i;
                break;
            }
        }
    }
    return QByteArray(str.data(), size);
}

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

    // Convert to UTF-8
    const QByteArray fileNameBuf = fileName.toUtf8();

    // Determine start position of file extension
    int dotPosition = fileNameBuf.size() < FILEEXTENSION_MAX_LENGTH ? 0 : fileNameBuf.size() - FILEEXTENSION_MAX_LENGTH;
    while (dotPosition < fileNameBuf.size() && fileNameBuf[dotPosition] != '.') {
        dotPosition += 1;
    }

    QByteArray baseName = fileNameBuf.left(dotPosition);
    if (baseName.isEmpty()) {
        baseName = QByteArrayLiteral("unnamed_file");
    }

    const QByteArray extension = fileNameBuf.mid(dotPosition);
    const QByteArray pathBuf = path.toUtf8() + '/';

    QByteArray result;
    QByteArray suffix = extension;
    int collisionCount = 1;

    do {
        result = pathBuf + cutUtf8String(baseName, FILENAME_MAX_LENGTH - suffix.size()) + suffix;
        collisionCount++;
        suffix = '(' + QByteArray::number(collisionCount) + ')' + extension;
    } while (QFileInfo::exists(result));

    return QString::fromUtf8(result);
}
