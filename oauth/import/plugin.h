/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

#ifndef SAILFISHOS_OAUTH_PLUGIN_H
#define SAILFISHOS_OAUTH_PLUGIN_H

#include <QtCore/QCoreApplication>
#include <QtCore/QTranslator>
#include <QtCore/QLocale>

#include <QtQml/QQmlExtensionPlugin>

namespace SailfishOS {

namespace OAuth {

class SailfishOSOAuthPlugin : public QQmlExtensionPlugin
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

} // namespace OAuth

} // namespace SailfishOS

#endif // SAILFISHOS_OAUTH_PLUGIN_H
