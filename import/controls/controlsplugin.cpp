/****************************************************************************
**
** Copyright (c) 2020 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExtensionPlugin>

#include <QtCore/QFileInfo>
#include <QtCore/QLocale>
#include <QtCore/QString>
#include <QtCore/QTranslator>
#include <QCoreApplication>

namespace SailfishOS {

namespace WebView {

namespace Controls {

// using custom translator so it gets properly removed from qApp when engine is deleted
class AppTranslator: public QTranslator
{
    Q_OBJECT

public:
    AppTranslator(QObject *parent)
        : QTranslator(parent)
    {
        QCoreApplication::instance()->installTranslator(this);
    }

    virtual ~AppTranslator()
    {
        QCoreApplication::instance()->removeTranslator(this);
    }
};

class SailfishOSWebViewTextSelectionPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override
    {
        Q_UNUSED(uri)
    }

    void initializeEngine(QQmlEngine *engine, const char *uri) override
    {
        Q_UNUSED(uri)

        AppTranslator *engineeringEnglish = new AppTranslator(engine);
        AppTranslator *translator = new AppTranslator(engine);
        engineeringEnglish->load("sailfish_components_webview_controls_qt5_eng_en", "/usr/share/translations");
        translator->load(QLocale(), "sailfish_components_webview_controls_qt5", "-", "/usr/share/translations");
    }
};

} // namespace TextSelection

} // namespace WebView

} // namespace SailfishOS

#include "controlsplugin.moc"
