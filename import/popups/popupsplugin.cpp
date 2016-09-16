/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Chris Adams <chris.adams@jollamobile.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "popupsplugin.h"

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

#include <QtCore/QFileInfo>
#include <QtCore/QString>

namespace SailfishOS {

namespace WebView {

namespace Popups {

QString DownloadHelper::createUniqueFileUrl(const QString &fileName, const QString &path) const
{
    if (path.isEmpty() || fileName.isEmpty()) {
        return QString();
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

static QObject *DownloadHelper_factory(QQmlEngine *, QJSEngine *)
{
    return new DownloadHelper;
}

void SailfishOSWebViewPopupsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.WebView.Popups"));
    qmlRegisterSingletonType<DownloadHelper>("Sailfish.WebView.Popups", 1, 0, "DownloadHelper", DownloadHelper_factory);
}

void SailfishOSWebViewPopupsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.WebView.Popups"));

    AppTranslator *engineeringEnglish = new AppTranslator(engine);
    AppTranslator *translator = new AppTranslator(engine);
    engineeringEnglish->load("sailfish_components_webview_popups_qt5_eng_en", "/usr/share/translations");
    translator->load(QLocale(), "sailfish_components_webview_popups_qt5", "-", "/usr/share/translations");
}

} // namespace Popups

} // namespace WebView

} // namespace SailfishOS

