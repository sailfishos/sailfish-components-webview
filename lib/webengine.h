/****************************************************************************
**
** Copyright (C) 2016-2026 Jolla Mobile Ltd
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
#include <QVariant>

#include <string>
#include <vector>

#ifndef Q_QDOC

namespace SailfishOS {

class WebEnginePrivate;

class WebEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool initialized READ isInitialized NOTIFY initialized)

public:
    typedef void (*TaskCallback)(void *data);
    typedef void *TaskHandle;

    static void initialize(const QString &profilePath, bool runEmbedding = true);
    static WebEngine *instance();

    explicit WebEngine(QObject *parent = 0);
    virtual ~WebEngine();

    Q_INVOKABLE bool isInitialized() const;
    Q_INVOKABLE bool isAccelerated() const;

    TaskHandle PostUITask(TaskCallback callback, void *data, int timeout = 0);
    TaskHandle PostCompositorTask(TaskCallback callback, void *data, int timeout = 0);
    void CancelTask(TaskHandle handle);

    void addObservers(const std::vector<std::string> &observers);
    void removeObservers(const std::vector<std::string> &observers);
    int getNumberOfWindows() const;

public slots:
    void setIsAccelerated(bool accelerated);
    void addObserver(const QString &topic);
    void removeObserver(const QString &topic);
    void notifyObservers(const QString &topic, const QString &value);
    void notifyObservers(const QString &topic, const QVariant &value);

    void addUserStyleSheet(const QUrl &url);
    void removeUserStyleSheet(const QUrl &url);

    void runEmbedding(int delay = -1);
    void stopEmbedding();
    void notifyFirstUIInitialized();

signals:
    void initialized();
    void contextDestroyed();
    void lastWindowDestroyed();
    void recvObserve(const QString message, const QVariant data);

private:
    WebEnginePrivate *d;
    Q_DISABLE_COPY(WebEngine)
};

}

#endif // !Q_QDOC
#endif // SAILFISHOS_WEBENGINE_H
