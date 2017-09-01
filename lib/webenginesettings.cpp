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
#include <QtGui/QStyleHints>

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
    static bool isInitialized = false;
    if (isInitialized) {
        return;
    }

    SailfishOS::WebEngineSettings *engineSettings = instance();

    // Standard settings.
    engineSettings->setPreference(QStringLiteral("embedlite.azpc.handle.singletap"), QVariant::fromValue<bool>(false));
    engineSettings->setPreference(QStringLiteral("embedlite.azpc.json.singletap"), QVariant::fromValue<bool>(true));
    engineSettings->setPreference(QStringLiteral("embedlite.azpc.handle.longtap"), QVariant::fromValue<bool>(false));
    engineSettings->setPreference(QStringLiteral("embedlite.azpc.json.longtap"), QVariant::fromValue<bool>(true));
    engineSettings->setPreference(QStringLiteral("embedlite.azpc.json.viewport"), QVariant::fromValue<bool>(true));
    // TODO: Fix this so that it can be applied during runtime when QQuickItem based WebView is used with QQuickFlickable.
    // At the moment just disable it to avoid unnecessary events being fired. JB#39581
#if 0
    engineSettings->setPreference(QStringLiteral("apz.asyncscroll.throttle"), QVariant::fromValue<int>(15));
    engineSettings->setPreference(QStringLiteral("apz.asyncscroll.timeout"), QVariant::fromValue<int>(15));
#endif
    engineSettings->setPreference(QStringLiteral("apz.fling_stopped_threshold"), QLatin1String("0.13f"));

    // Theme settings.
    engineSettings->setPreference(QStringLiteral("ui.textSelectBackground"), QLatin1String("#878787"));

    // Make long press timeout equal to the one in Qt
    engineSettings->setPreference(QStringLiteral("ui.click_hold_context_menus.delay"), QVariant(PressAndHoldDelay));

    // DPI is passed to Gecko's View and APZTreeManager.
    // Touch tolerance is calculated with formula: dpi * tolerance = pixel threshold
    const int dragThreshold = QGuiApplication::styleHints()->startDragDistance();
    qreal touchStartTolerance = dragThreshold / QGuiApplication::primaryScreen()->physicalDotsPerInch();
    engineSettings->setPreference(QString("apz.touch_start_tolerance"), QString("%1f").arg(touchStartTolerance));

    Silica::Theme *silicaTheme = Silica::Theme::instance();

    qreal pixelRatio = SAILFISH_WEBENGINE_DEFAULT_PIXEL_RATIO * silicaTheme->pixelRatio();
    // Round to nearest even rounding factor
    pixelRatio = qRound(pixelRatio / 0.5) * 0.5;

    int screenWidth = QGuiApplication::primaryScreen()->size().width();

    if (!testScreenDimensions(pixelRatio)) {
        qreal tempPixelRatio = qFloor(pixelRatio);
        if (testScreenDimensions(tempPixelRatio)) {
            pixelRatio = tempPixelRatio;
        }
    } else if (screenWidth >= 1080) {
        pixelRatio = qRound(pixelRatio);
    }

    engineSettings->setPixelRatio(pixelRatio);

    int tileSize = screenWidth;

    // With bigger than FullHD screen fill with two tiles in row (portrait).
    // Landscape will be filled with same tile size.
    if (screenWidth > 1080) {
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
    engineSettings->setPreference(QStringLiteral("browser.enable_automatic_image_resizing"),
                                  QVariant::fromValue<bool>(true));

    isInitialized = true;
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
