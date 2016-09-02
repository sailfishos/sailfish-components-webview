/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Chris Adams <chris.adams@jollamobile.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAILFISHOS_WEBVIEW_PICKERS_PLUGIN_H
#define SAILFISHOS_WEBVIEW_PICKERS_PLUGIN_H

#include <QtQml/QQmlExtensionPlugin>
#include <QtQuick/QQuickItem>

#include <QtCore/QTranslator>
#include <QtCore/QLocale>

namespace SailfishOS {

namespace WebView {

namespace Pickers {

class SailfishOSWebViewPickersPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

// using custom translator so it gets properly removed from qApp when engine is deleted
class AppTranslator: public QTranslator
{
    Q_OBJECT

public:
    AppTranslator(QObject *parent)
        : QTranslator(parent)
    {
        qApp->installTranslator(this);
    }

    virtual ~AppTranslator()
    {
        qApp->removeTranslator(this);
    }
};

} // namespace Pickers

} // namespace WebView

} // namespace SailfishOS

#endif // SAILFISHOS_WEBVIEW_PICKERS_PLUGIN_H
