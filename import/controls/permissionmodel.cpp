/****************************************************************************
**
** Copyright (c) 2020 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "permissionmodel.h"
#include "webengine.h"

static const auto PERMS_ALL = QStringLiteral("embed:perms:all");

PermissionModel::PermissionModel(QObject *parent) : QAbstractListModel(parent)
{
    SailfishOS::WebEngine *webEngine = SailfishOS::WebEngine::instance();
    connect(webEngine, &SailfishOS::WebEngine::recvObserve, this, &PermissionModel::handleRecvObserve);
    webEngine->addObserver(PERMS_ALL);

    PermissionManager::sendRequest(QStringLiteral("get-all"));
}

void PermissionModel::add(const QString &host, const QString &type, int capability)
{
    int count = m_permissionList.count();
    beginInsertRows(QModelIndex(), count, count);

    Permission permission(host, type, PermissionManager::intToCapability(capability));
    PermissionManager::add(permission);

    if (!m_permissionList.contains(permission))
        m_permissionList.append(permission);

    endInsertRows();
}

QVariant PermissionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_permissionList.count())
        return QVariant();

    switch (role) {
    case Uri:
        return m_permissionList.at(index.row()).m_host;
    case Type:
        return m_permissionList.at(index.row()).m_type;
    case Capability:
        return m_permissionList.at(index.row()).m_capability;
    default:
        return QVariant();
    }
}

int PermissionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_permissionList.count();
}

QHash<int, QByteArray> PermissionModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Uri] = "uri";
    roles[Type] = "type";
    roles[Capability] = "capability";
    return roles;
}

void PermissionModel::handleRecvObserve(const QString &message, const QVariant &data)
{
    if (message == PERMS_ALL) {
        setPermissionList(qvariant_cast<QVariantList>(data));
    }
}

void PermissionModel::setPermissionList(const QVariantList &data)
{
    beginResetModel();

    m_permissionList.clear();

    for (const auto &iter : data) {
        QVariantMap varMap = iter.toMap();
        m_permissionList.append(Permission(varMap.value("uri").toString(),
                                           varMap.value("type").toString(),
                                           PermissionManager::intToCapability(varMap.value("capability").toInt())));
    }
    endResetModel();
}

void PermissionModel::remove(int index)
{
    beginRemoveRows(QModelIndex(), index, index);

    Permission permission = m_permissionList.at(index);
    PermissionManager::remove(permission.m_host, permission.m_type);
    m_permissionList.removeAt(index);

    endRemoveRows();
}

void PermissionModel::setCapability(QModelIndex index, int capability)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_permissionList.count())
        return;

    Permission &permission = m_permissionList[index.row()];
    permission.m_capability = PermissionManager::intToCapability(capability);
    PermissionManager::add(permission);
    QVector<int> roles = {Capability};
    dataChanged(index, index, roles);
}
