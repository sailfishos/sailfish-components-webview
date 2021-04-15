/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

#ifndef SAILFISHOS_OAUTH_OAUTHSYMBOLS_H
#define SAILFISHOS_OAUTH_OAUTHSYMBOLS_H

#ifdef SAILFISHOS_OAUTH_LIBRARY_BUILD
#define OAUTH_SYMBOL Q_DECL_EXPORT
#else
#define OAUTH_SYMBOL Q_DECL_IMPORT
#endif

#endif // SAILFISHOS_OAUTH_OAUTHSYMBOLS_H
