/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

#ifndef SAILFISHOS_OAUTH_REDIRECTURIPARSER_H
#define SAILFISHOS_OAUTH_REDIRECTURIPARSER_H

#include "oauthsymbols.h"

#include <QtCore/QVariantMap>
#include <QtCore/QString>

namespace SailfishOS {

namespace OAuth {

QVariantMap parseRedirectUri(const QString &redirectUri, QString *errorMessage) OAUTH_SYMBOL;

}

}

#endif // SAILFISHOS_OAUTH_REDIRECTURIPARSER_H
