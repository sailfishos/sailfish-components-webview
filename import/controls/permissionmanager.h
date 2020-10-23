/****************************************************************************
**
** Copyright (c) 2020 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PERMISSIONMANAGER_H
#define PERMISSIONMANAGER_H

#include <QObject>
#include <QJSValue>
#include <QScopedPointer>

class Permission;

class PermissionManager : public QObject
{
    Q_OBJECT
public:
    PermissionManager(QObject *parent = nullptr);

    // See https://developer.mozilla.org/en-US/docs/Mozilla/Tech/XPCOM/Reference/Interface/nsIPermissionManager
    // And https://git.sailfishos.org/mer-core/gecko-dev/blob/master/netwerk/base/nsIPermissionManager.idl
    enum Capability {
        Unknown = 0,
        Allow = 1,
        Deny = 2,
        Prompt = 3
    };
    Q_ENUM(Capability)

    Q_INVOKABLE void requestUriPermissions(const QString &uri, QJSValue callback);

    static void add(const Permission &permission);
    static void remove(const QString &host, const QString &type);
    static void sendRequest(const QString &message,
                            const QString &host = QString(),
                            const QString &type = QString(),
                            Capability capability = Unknown);

    static int capabilityToInt(Capability capability);
    static Capability intToCapability(int value);

private slots:
    void handleRecvObserve(const QString &message, const QVariant &data);

private:
    QScopedPointer<QJSValue> m_callback;
};

#endif // PERMISSIONMANAGER_H
