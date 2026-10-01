// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
// SPDX-License-Identifier: MPL-2.0
#pragma once
#include <QObject>
#include <QCoreApplication>
#include <QVariant>
#include <QSet>
#include <QStringList>
#include <map>
#include <string>
#include <vector>

namespace mozilla { namespace embedlite {
class EmbedLiteMessagePump {};
class EmbedLiteAppListener {
public:
    virtual ~EmbedLiteAppListener() {}
    virtual void Initialized() {}
    virtual void Destroyed() {}
    virtual void OnObserve(const char *, const char16_t *) {}
    virtual void LastWindowDestroyed() {}
};
class EmbedLiteApp {
public:
    EmbedLiteAppListener *listener = nullptr;
    bool accelerated = true;
    int windows = 0;
    QSet<QString> observers;
    void SetProfilePath(const char *path) {
        qApp->setProperty("abi.profile", QString::fromUtf8(path));
    }
    void AddManifestLocation(const char *path) {
        QStringList paths = qApp->property("abi.manifests").toStringList();
        paths.append(QString::fromUtf8(path));
        qApp->setProperty("abi.manifests", paths);
    }
    void AddObserver(const char *topic) { observers.insert(QString::fromUtf8(topic)); }
    void RemoveObserver(const char *topic) { observers.remove(QString::fromUtf8(topic)); }
    void AddObservers(const std::vector<std::string> &topics) {
        for (const auto &topic : topics) AddObserver(topic.c_str());
    }
    void RemoveObservers(const std::vector<std::string> &topics) {
        for (const auto &topic : topics) RemoveObserver(topic.c_str());
    }
    void SendObserve(const char *topic, const char16_t *data) {
        if (listener && observers.contains(QString::fromUtf8(topic)))
            listener->OnObserve(topic, data ? data : u"");
    }
    void SetBoolPref(const char *key, bool value) { qApp->setProperty(key, value); }
    void SetIntPref(const char *key, int value) { qApp->setProperty(key, value); }
    void SetCharPref(const char *key, const char *value) {
        qApp->setProperty(key, QString::fromUtf8(value));
    }
    void LoadGlobalStyleSheet(const char *, bool) {}
    void LoadUserStyleSheet(const char *uri, bool enable) {
        qApp->setProperty(uri, enable);
    }
    int GetNumberOfWindows() const { return windows; }
    void *PostTask(void (*cb)(void *), void *data, int) { cb(data); return nullptr; }
    void *PostCompositorTask(void (*cb)(void *), void *data, int timeout) {
        return PostTask(cb, data, timeout);
    }
    void CancelTask(void *) {}
    void SetIsAccelerated(bool value) { accelerated = value; }
    bool IsAccelerated() const { return accelerated; }
};
}}
