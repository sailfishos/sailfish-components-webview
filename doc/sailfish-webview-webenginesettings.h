/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QObject>

// This file is used only for documentation purposes

// These items are all part of QMozEngineSettings, but inherited by
// WebEngineSettings, so we take some notational liberty and present them
// as part of the the subclass.

namespace SailfishOS {

class WebEngineSettings : public QMozEngineSettings {
    Q_OBJECT
    Q_PROPERTY(bool autoLoadImages READ autoLoadImages WRITE setAutoLoadImages NOTIFY autoLoadImagesChanged FINAL)
    Q_PROPERTY(bool javascriptEnabled READ javascriptEnabled WRITE setJavascriptEnabled NOTIFY javascriptEnabledChanged FINAL)
    Q_PROPERTY(bool popupEnabled READ popupEnabled WRITE setPopupEnabled NOTIFY popupEnabledChanged)
    Q_PROPERTY(CookieBehavior cookieBehavior READ cookieBehavior WRITE setCookieBehavior NOTIFY cookieBehaviorChanged)
    Q_PROPERTY(bool useDownloadDir READ useDownloadDir WRITE setUseDownloadDir NOTIFY useDownloadDirChanged)
    Q_PROPERTY(QString downloadDir READ downloadDir WRITE setDownloadDir NOTIFY downloadDirChanged)
    Q_PROPERTY(qreal pixelRatio READ pixelRatio WRITE setPixelRatio NOTIFY pixelRatioChanged)
    Q_PROPERTY(bool initialized READ isInitialized NOTIFY initialized)

public:
    // C++ API
    static void initialize();
    static WebEngineSettings *instance();

    explicit WebEngineSettings(QObject *parent = 0);
    virtual ~WebEngineSettings();

    // See developer.mozilla.org/en-US/docs/Mozilla/Firefox/Privacy/Storage_access_policy
    // And git.sailfishos.org/mer-core/gecko-dev/blob/master/netwerk/cookie/nsCookieService.cpp
    enum CookieBehavior {
        AcceptAll = 0,
        BlockThirdParty = 1,
        BlockAll = 2,
        Deprecated = 3
    };
    Q_ENUM(CookieBehavior)

    bool isInitialized() const;

    bool autoLoadImages() const;
    void setAutoLoadImages(bool enabled);

    bool javascriptEnabled() const;
    void setJavascriptEnabled(bool enabled);

    bool popupEnabled() const;
    void setPopupEnabled(bool enabled);

    CookieBehavior cookieBehavior() const;
    void setCookieBehavior(CookieBehavior cookieBehavior);

    bool useDownloadDir() const;
    void setUseDownloadDir(bool useDownloadDir);

    QString downloadDir() const;
    void setDownloadDir(const QString &downloadDir);

    void setTileSize(const QSize &size);

    void setPixelRatio(qreal pixelRatio);
    qreal pixelRatio() const;

    void enableProgressivePainting(bool enabled);
    void enableLowPrecisionBuffers(bool enabled);

    // Low-level API to set engine preferences.
    Q_INVOKABLE void setPreference(const QString &key, const QVariant &value);

Q_SIGNALS:
    void autoLoadImagesChanged();
    void javascriptEnabledChanged();
    void popupEnabledChanged();
    void cookieBehaviorChanged();
    void useDownloadDirChanged();
    void downloadDirChanged();
    void initialized();
    void pixelRatioChanged();
};

} // namespace SailfishOS
