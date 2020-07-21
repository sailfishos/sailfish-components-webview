/****************************************************************************
**
** Copyright (c) 2020 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "permissionmanager.h"
#include "permissionmodel.h"
#include "webengine.h"

#include <QJSEngine>

static const auto PERMS_ALL_FOR_URI = QStringLiteral("embed:perms:all-for-uri");

PermissionManager::PermissionManager(QObject *parent)
    : QObject(parent)
{
    SailfishOS::WebEngine *webEngine = SailfishOS::WebEngine::instance();
    connect(webEngine, &SailfishOS::WebEngine::recvObserve, this, &PermissionManager::handleRecvObserve);
    webEngine->addObserver(PERMS_ALL_FOR_URI);
}

void PermissionManager::add(const Permission &permission)
{
    sendRequest(QStringLiteral("add"), permission.m_host, permission.m_type, permission.m_capability);
}

void PermissionManager::remove(const QString &host, const QString &type)
{
    sendRequest(QStringLiteral("remove"), host, type);
}

void PermissionManager::requestUriPermissions(const QString &uri, QJSValue callback)
{
    if (!callback.isNull() && !callback.isUndefined() && callback.isCallable()) {
        m_callback.reset(new QJSValue(callback));
    }
    sendRequest(QStringLiteral("get-all-for-uri"), uri);
}

void PermissionManager::sendRequest(const QString &message
                               , const QString &host
                               , const QString &type
                               , Capability capability)
{
    QVariantMap data;
    data.insert(QStringLiteral("msg"), message);
    data.insert(QStringLiteral("uri"), host);
    data.insert(QStringLiteral("type"), type);
    data.insert(QStringLiteral("permission"), QVariant::fromValue(capabilityToInt(capability)));
    SailfishOS::WebEngine::instance()->notifyObservers("embedui:perms", QVariant(data));
}

int PermissionManager::capabilityToInt(PermissionManager::Capability capability)
{
    return static_cast<int>(capability);
}

PermissionManager::Capability PermissionManager::intToCapability(int value)
{
    return static_cast<Capability>(value);
}

void PermissionManager::handleRecvObserve(const QString &message, const QVariant &data)
{
    if (message == PERMS_ALL_FOR_URI) {
        m_callback->call(QJSValueList() << m_callback->engine()->toScriptValue(data));
        m_callback.reset();
    }
}
