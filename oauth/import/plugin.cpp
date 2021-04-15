/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

#include "plugin.h"
#include "oauth1.h"
#include "oauth2.h"
#include "redirectlistener.h"

#include <QtQml>

namespace SailfishOS {

namespace OAuth {

void SailfishOSOAuthPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.OAuth"));
    qmlRegisterType<SailfishOS::OAuth::OAuth1>("Sailfish.OAuth", 1, 0, "OAuth1");
    qmlRegisterType<SailfishOS::OAuth::OAuth2>("Sailfish.OAuth", 1, 0, "OAuth2");
    qmlRegisterType<SailfishOS::OAuth::RedirectListener>("Sailfish.OAuth", 1, 0, "RedirectListener");
    qmlRegisterUncreatableType<SailfishOS::OAuth::Error>("Sailfish.OAuth", 1, 0, "Error",
            QStringLiteral("You cannot construct an OAuth error object in QML"));

    qRegisterMetaType<SailfishOS::OAuth::Error>("SailfishOS::OAuth::Error");
}

void SailfishOSOAuthPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Sailfish.OAuth"));

    AppTranslator *engineeringEnglish = new AppTranslator(engine);
    AppTranslator *translator = new AppTranslator(engine);
    engineeringEnglish->load("sailfish_components_oauth_eng_en", "/usr/share/translations");
    translator->load(QLocale(), "sailfish_components_oauth", "-", "/usr/share/translations");
}

} // namespace WebView

} // namespace SailfishOS

