/****************************************************************************
**
** Copyright (c) 2016 Jolla Ltd.
** Copyright (c) 2020 Open Mobile Platform LLC.
** Contact: Raine Makelainen <raine.makelaine@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAILFISHOS_WEBENGINE_H
#define SAILFISHOS_WEBENGINE_H

#include <QString>
#include <qmozcontext.h>

#ifndef Q_QDOC

namespace SailfishOS {

class WebEngine : public QMozContext {
    Q_OBJECT
public:
    static void initialize(const QString &profilePath, bool runEmbedding = true);
    static WebEngine *instance();

    explicit WebEngine(QObject *parent = 0);
    virtual ~WebEngine();
};
}

#endif // !Q_QDOC
#endif // SAILFISHOS_WEBENGINE_H
