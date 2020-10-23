/****************************************************************************
**
** Copyright (c) 2020 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PERMISSIONMODEL_H
#define PERMISSIONMODEL_H

#include <QAbstractListModel>
#include "permissionmanager.h"

struct Permission
{
    Permission(QString host, QString type, PermissionManager::Capability capability)
        : m_host(host)
        , m_type(type)
        , m_capability(capability)
    {
    }

    bool operator==(const Permission &other) const
    {
        return (m_host == other.m_host && m_type == other.m_type && m_capability == other.m_capability);
    }

    QString m_host;
    QString m_type;
    PermissionManager::Capability m_capability;
};

class PermissionModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        Uri = Qt::UserRole,
        Type,
        Capability
    };

    PermissionModel(QObject *parent = nullptr);

    /* Add host to exclusion list. The type property can be "geolocation", "cookie",
     * "desktop-notification, "popup", etc. */
    Q_INVOKABLE void add(const QString &host, const QString &type, int capability);

    void setCapability(QModelIndex index, int capability);
    void remove(int index);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    void setPermissionList(const QVariantList &data);

private slots:
    void handleRecvObserve(const QString &message, const QVariant &data);

private:
    QList<Permission> m_permissionList;
};

#endif // PERMISSIONMODEL_H
