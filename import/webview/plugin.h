/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Chris Adams <chris.adams@jollamobile.com>
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
#include <QtQuick/QQuickItem>

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
};

class RawWebView : public QuickMozView
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *flickable READ flickable WRITE setFlickable NOTIFY flickableChanged FINAL)

public:
    RawWebView(QQuickItem *parent = 0);
    ~RawWebView();

    QQuickItem *flickable() const;
    void setFlickable(QQuickItem *flickable);
    bool eventFilter(QObject *object, QEvent *event);

Q_SIGNALS:
    void flickableChanged();

private:
    QQuickItem *m_flickable;
    QPointF m_startPos;
    QPointF m_movePos;
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
