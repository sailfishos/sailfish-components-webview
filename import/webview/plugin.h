/****************************************************************************
**
** Copyright (C) 2021 Jolla Ltd.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAILFISHOS_WEBVIEW_PLUGIN_H
#define SAILFISHOS_WEBVIEW_PLUGIN_H

#include <QtCore/QTranslator>
#include <QtCore/QLocale>

#include <QtQml/QQmlExtensionPlugin>

//mozembedlite-qt5
#include <quickmozview.h>

namespace SailfishOS {

namespace WebView {

class SailfishOSWebViewPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);

private:
    void initUserAgentOverrides(const QString &path);
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

} // namespace WebView

} // namespace SailfishOS

#endif // SAILFISHOS_WEBVIEW_PLUGIN_H
