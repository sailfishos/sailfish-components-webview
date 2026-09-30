/****************************************************************************
**
** Copyright (c) 2016 - 2021 Jolla Ltd.
** Copyright (c) 2024 - 2026 Jolla Mobile Ltd
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAILFISHOS_WEBENGINE_H
#define SAILFISHOS_WEBENGINE_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <string>
#include <vector>
#include <qmozcontext.h>

#ifndef Q_QDOC

namespace SailfishOS {

class WebEngine : public QMozContext
{
    Q_OBJECT
public:
    static void initialize(const QString &profilePath, bool runEmbedding = true);
    static WebEngine *instance();

    explicit WebEngine(QObject *parent = 0);
    virtual ~WebEngine();

public slots:
    void addUserStyleSheet(const QUrl &url);
    void removeUserStyleSheet(const QUrl &url);
};

}

#endif // !Q_QDOC
#endif // SAILFISHOS_WEBENGINE_H
