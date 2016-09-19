/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Raine Makelainen <raine.makelaine@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "webenginesettings.h"

#include <silicatheme.h>

#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QSize>
#include <QtCore/QtMath>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

Q_GLOBAL_STATIC(SailfishOS::WebEngineSettings, webEngineSettingsInstance)

#define SAILFISH_WEBENGINE_DEFAULT_PIXEL_RATIO 1.5

static bool testScreenDimensions(qreal pixelRatio)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    qreal w = screen->size().width() / pixelRatio;
    qreal h = screen->size().height() / pixelRatio;
    return fmod(w, 1.0) == 0 && fmod(h, 1.0) == 0;
}

const QSettings &quickSettings()
{
    static const QSettings settings(QSettings::SystemScope, QStringLiteral("QtProject"), QStringLiteral("QtQuick2"));
    return settings;
}

int getPressAndHoldDelay()
{
    return quickSettings().value(QStringLiteral("QuickMouseArea/PressAndHoldDelay"), 800).toInt();
}

const int PressAndHoldDelay(getPressAndHoldDelay());

void SailfishOS::WebEngineSettings::initialize()
{
    SailfishOS::WebEngineSettings *engineSettings = instance();

    // Standard settings.
    engineSettings->setPreference(QStringLiteral("embedlite.azpc.handle.singletap"), QVariant::fromValue<bool>(false));
    engineSettings->setPreference(QStringLiteral("embedlite.azpc.json.singletap"), QVariant::fromValue<bool>(true));
    engineSettings->setPreference(QStringLiteral("embedlite.azpc.handle.longtap"), QVariant::fromValue<bool>(false));
    engineSettings->setPreference(QStringLiteral("embedlite.azpc.json.longtap"), QVariant::fromValue<bool>(true));
    engineSettings->setPreference(QStringLiteral("embedlite.azpc.json.viewport"), QVariant::fromValue<bool>(true));
    engineSettings->setPreference(QStringLiteral("apz.asyncscroll.throttle"), QVariant::fromValue<int>(400));
    engineSettings->setPreference(QStringLiteral("apz.asyncscroll.timeout"), QVariant::fromValue<int>(400));
    engineSettings->setPreference(QStringLiteral("apz.fling_stopped_threshold"), QLatin1String("0.13f"));

    // Make long press timeout equal to the one in Qt
    engineSettings->setPreference(QStringLiteral("ui.click_hold_context_menus.delay"), QVariant(PressAndHoldDelay));

    Silica::Theme *silicaTheme = Silica::Theme::instance();

    qreal pixelRatio = SAILFISH_WEBENGINE_DEFAULT_PIXEL_RATIO * silicaTheme->pixelRatio();
    // Round to nearest even rounding factor
    pixelRatio = qRound(pixelRatio / 0.5) * 0.5;

    // If we're on hdpi and calcaluted pixel ratio doesn't result integer dimensions, let's try to floor it.
    if (pixelRatio >= 2.0 && !testScreenDimensions(pixelRatio)) {
        qreal tempPixelRatio = qFloor(pixelRatio);
        if (testScreenDimensions(tempPixelRatio)) {
            pixelRatio = tempPixelRatio;
        }
    }

    engineSettings->setPixelRatio(pixelRatio);

    int screenWidth = QGuiApplication::primaryScreen()->size().width();
    int tileSize = screenWidth;

    // With bigger than qHD screen fill with two tiles in row (portrait).
    // Landscape will be filled with same tile size.
    if (screenWidth > 540) {
        tileSize = screenWidth / 2;
    }
    engineSettings->setTileSize(QSize(tileSize, tileSize));

    // Zooming related preferences.
    engineSettings->setPreference(QStringLiteral("embedlite.zoomMargin"),
                                  QVariant::fromValue<qreal>(silicaTheme->paddingMedium()));
    engineSettings->setPreference(QStringLiteral("embedlite.inputItemSize"),
                                  QVariant::fromValue<qreal>(silicaTheme->fontSizeSmall()));

    // Infer and set Accept-Language header from the current system locale
    QString langs;
    QStringList locale = QLocale::system().name().split("_", QString::SkipEmptyParts);
    if (locale.size() > 1) {
        langs = QString("%1-%2,%3").arg(locale.at(0)).arg(locale.at(1)).arg(locale.at(0));
    } else {
        langs = locale.at(0);
    }
    engineSettings->setPreference(QStringLiteral("intl.accept_languages"),
                                  QVariant::fromValue<QString>(langs));

}

SailfishOS::WebEngineSettings *SailfishOS::WebEngineSettings::instance()
{
    return webEngineSettingsInstance();
}

SailfishOS::WebEngineSettings::WebEngineSettings(QObject *parent)
    : QMozEngineSettings(parent)
{
}

SailfishOS::WebEngineSettings::~WebEngineSettings()
{

}
