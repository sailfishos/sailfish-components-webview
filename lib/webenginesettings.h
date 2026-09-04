/****************************************************************************
**
** Copyright (C) 2016-2026 Jolla Mobile Ltd
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAILFISHOS_WEBENGINE_SETTINGS_H
#define SAILFISHOS_WEBENGINE_SETTINGS_H

#include <QObject>
#include <QSize>
#include <QString>
#include <QVariant>

#ifndef Q_QDOC

namespace SailfishOS {

class WebEngineSettingsPrivate;

class WebEngineSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool initialized READ isInitialized NOTIFY initialized)
    Q_PROPERTY(bool autoLoadImages READ autoLoadImages WRITE setAutoLoadImages NOTIFY autoLoadImagesChanged FINAL)
    Q_PROPERTY(bool javascriptEnabled READ javascriptEnabled WRITE setJavascriptEnabled NOTIFY javascriptEnabledChanged FINAL)
    Q_PROPERTY(bool popupEnabled READ popupEnabled WRITE setPopupEnabled NOTIFY popupEnabledChanged)
    Q_PROPERTY(CookieBehavior cookieBehavior READ cookieBehavior WRITE setCookieBehavior NOTIFY cookieBehaviorChanged)
    Q_PROPERTY(bool useDownloadDir READ useDownloadDir WRITE setUseDownloadDir NOTIFY useDownloadDirChanged)
    Q_PROPERTY(QString downloadDir READ downloadDir WRITE setDownloadDir NOTIFY downloadDirChanged)
    Q_PROPERTY(qreal pixelRatio READ pixelRatio WRITE setPixelRatio NOTIFY pixelRatioChanged)
    Q_PROPERTY(bool doNotTrack READ doNotTrack WRITE setDoNotTrack NOTIFY doNotTrackChanged)
    Q_PROPERTY(ColorScheme colorScheme READ colorScheme WRITE setColorScheme NOTIFY colorSchemeChanged)

public:
    enum CookieBehavior {
        AcceptAll = 0,
        BlockThirdParty = 1,
        BlockAll = 2,
        Deprecated = 3
    };
    Q_ENUM(CookieBehavior)

    enum ColorScheme {
        PrefersLightMode = 0,
        PrefersDarkMode = 1,
        FollowsAmbience = 2
    };
    Q_ENUM(ColorScheme)

    enum PreferenceType {
        UnknownPref = 0,
        StringPref = 32,
        IntPref = 64,
        BoolPref = 128
    };
    Q_ENUM(PreferenceType)

    static void initialize();
    static WebEngineSettings *instance();

    explicit WebEngineSettings(QObject *parent = 0);
    virtual ~WebEngineSettings();

    bool isInitialized() const;
    bool autoLoadImages() const;
    void setAutoLoadImages(bool enabled);
    bool javascriptEnabled() const;
    void setJavascriptEnabled(bool enabled);
    bool popupEnabled() const;
    void setPopupEnabled(bool enabled);
    CookieBehavior cookieBehavior() const;
    void setCookieBehavior(CookieBehavior behavior);
    bool useDownloadDir() const;
    void setUseDownloadDir(bool useDownloadDir);
    QString downloadDir() const;
    void setDownloadDir(const QString &downloadDir);
    void setTileSize(const QSize &size);
    void setPixelRatio(qreal pixelRatio);
    qreal pixelRatio() const;
    bool doNotTrack() const;
    void setDoNotTrack(bool doNotTrack);
    ColorScheme colorScheme() const;
    void setColorScheme(ColorScheme colorScheme);
    void enableProgressivePainting(bool enabled);
    void enableLowPrecisionBuffers(bool enabled);

    Q_INVOKABLE void setPreference(const QString &key, const QVariant &value);
    Q_INVOKABLE void setPreference(const QString &key, const QVariant &value,
                                   PreferenceType preferenceType);

signals:
    void autoLoadImagesChanged();
    void javascriptEnabledChanged();
    void popupEnabledChanged();
    void cookieBehaviorChanged();
    void useDownloadDirChanged();
    void downloadDirChanged();
    void initialized();
    void pixelRatioChanged();
    void doNotTrackChanged();
    void colorSchemeChanged();

private:
    WebEngineSettingsPrivate *d;
    Q_DISABLE_COPY(WebEngineSettings)
};

}

#endif // !Q_QDOC
#endif // SAILFISHOS_WEBENGINE_SETTINGS_H
