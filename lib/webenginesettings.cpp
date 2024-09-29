/****************************************************************************
**
** Copyright (C) 2021 Jolla Ltd.
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "webengine.h"
#include "webenginesettings.h"
#include "webenginesettings_p.h"

#include <silicatheme.h>

#include <QtCore/QFile>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QSize>
#include <QtCore/QStandardPaths>
#include <QtCore/QtMath>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QStyleHints>
#include <sys/sysinfo.h>

#include "logging.h"

Q_GLOBAL_STATIC(SailfishOS::WebEngineSettings, webEngineSettingsInstance)
Q_GLOBAL_STATIC(SailfishOS::WebEngineSettingsPrivate, webEngineSettingsPrivateInstance)

#define SAILFISH_WEBENGINE_DEFAULT_PIXEL_RATIO 1.5

/*!
    \class SailfishOS::WebEngineSettings
    \brief Provides access to the global web engine settings.
    \inmodule SailfishWebView
    \inherits QMozEngineSettings

    Singleton class which provides access to the global Web engine settings.
*/

static bool testScreenDimensions(qreal pixelRatio)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    qreal w = screen->size().width() / pixelRatio;
    qreal h = screen->size().height() / pixelRatio;
    return fmod(w, 1.0) == 0 && fmod(h, 1.0) == 0;
}

static quint64 getTotalMemory() {
    struct sysinfo info;
    sysinfo(&info);
    return info.totalram;
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

SailfishOS::WebEngineSettingsPrivate *SailfishOS::WebEngineSettingsPrivate::instance()
{
    return webEngineSettingsPrivateInstance();
}

SailfishOS::WebEngineSettingsPrivate::WebEngineSettingsPrivate(QObject *parent)
    : QObject(parent)
{
}

SailfishOS::WebEngineSettingsPrivate::~WebEngineSettingsPrivate()
{
}


/*!
    \brief Initialises the WebEngineSettings class.

    The \c initialize method sets up default gecko preferences needed in order
    for the WebView to work correctly.

    If you need to set specific settings which differ from the default, it's
    recommended that you do so after this initialisation has been called, to
    avoid your settings being overwritten by it.

    This will be called automatically during QML initialisation, but it can
    also be called manually in your code in order to control the order of
    initialisation. Calling this method after intialialisation has already
    been performed has no effect.

    Hence the following is safe:

    \code
        SailfishOS::WebEngine::instance() *engineSettings = SailfishOS::WebEngineSettings::instance();
        engineSettings->initialize();
        engineSettings->setPreference(QStringLiteral("ui.textSelectBackground"), QLatin1String("#55ff55"));
    \endcode
*/
void SailfishOS::WebEngineSettings::initialize()
{
    static bool isInitialized = false;
    if (isInitialized) {
        return;
    }

    SailfishOS::WebEngineSettings *engineSettings = instance();

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

    // Ensure the renderer is configured correctly
    engineSettings->setPreference(QStringLiteral("gfx.webrender.force-disabled"),
                                  QVariant(true));
    engineSettings->setPreference(QStringLiteral("embedlite.compositor.external_gl_context"),
                                  QVariant(false));

    Silica::Theme *silicaTheme = Silica::Theme::instance();

    // Notify gecko when the ambience switches between light and dark
    if (engineSettings->isInitialized()) {
        engineSettings->d->notifyColorSchemeChanged();
    } else {
        connect(engineSettings, &QMozEngineSettings::initialized,
                engineSettings->d, &SailfishOS::WebEngineSettingsPrivate::notifyColorSchemeChanged);
    }
    connect(silicaTheme, &Silica::Theme::colorSchemeChanged,
            engineSettings->d, &SailfishOS::WebEngineSettingsPrivate::notifyColorSchemeChanged);

    // Subscribe to gecko messages
    // When the embedliteviewcreated notification is received the ambience notification will be sent again
    SailfishOS::WebEngine *webEngine = SailfishOS::WebEngine::instance();
    connect(webEngine, &SailfishOS::WebEngine::recvObserve,
            engineSettings->d, &SailfishOS::WebEngineSettingsPrivate::oneShotNotifyColorSchemeChanged);
    webEngine->addObserver(QStringLiteral("embedliteviewcreated"));

    isInitialized = true;

    // Guard preferences that should be written only once. If a preference needs to be
    // forcefully written upon each start that should happen before this.
    QString appConfig = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QFile markerFile(QString("%1/__PREFS_WRITTEN__").arg(appConfig));
    if (markerFile.exists()) {
        return;
    }

    // Standard settings.
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

    // Disable wasm_baselinejit to avoid crashes on lower memory devices; see JB#56635
    quint64 totalMemory = getTotalMemory();
    if (totalMemory < (2.5 * 1024 * 1024 * 1024)) {
        // Devices with roughly 2Gb and lower
        qCInfo(lcWebengineLog) << "Lower memory: disabling wasm_baselinejit";
        engineSettings->setPreference(QStringLiteral("javascript.options.wasm_baselinejit"), false);
    }

    // DPI is passed to Gecko's View and APZTreeManager.
    // Touch tolerance is calculated with formula: dpi * tolerance = pixel threshold
    const int dragThreshold = QGuiApplication::styleHints()->startDragDistance();
    qreal touchStartTolerance = dragThreshold / QGuiApplication::primaryScreen()->physicalDotsPerInch();
    engineSettings->setPreference(QString("apz.touch_start_tolerance"), QString("%1f").arg(touchStartTolerance));

    qreal pixelRatio = SAILFISH_WEBENGINE_DEFAULT_PIXEL_RATIO * silicaTheme->pixelRatio();
    // Round to nearest even rounding factor
    pixelRatio = qRound(pixelRatio / 0.5) * 0.5;

    int screenWidth = QGuiApplication::primaryScreen()->size().width();

    // Do not floor the pixel ratio if the pixel ratio less than 2.0 (1.5 is minimum).
    if (pixelRatio >= 2.0 && !testScreenDimensions(pixelRatio)) {
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

    engineSettings->setPreference(QStringLiteral("browser.enable_automatic_image_resizing"),
                                  QVariant::fromValue<bool>(true));

    // Enable user agent overrides
    engineSettings->setPreference(QStringLiteral("general.useragent.updates.enabled"),
                                  QVariant::fromValue<bool>(true));

    markerFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
    markerFile.close();
}

/*!
    \internal
    \brief Notifies gecko about ambience color scheme changes.
*/
void SailfishOS::WebEngineSettingsPrivate::notifyColorSchemeChanged()
{
    Silica::Theme *silicaTheme = Silica::Theme::instance();
    QString scheme = silicaTheme->colorScheme() == Silica::Theme::LightOnDark
            ? QStringLiteral("dark")
            : QStringLiteral("light");
    SailfishOS::WebEngine::instance()->notifyObservers(QStringLiteral("ambience-theme-changed"), scheme);
}

/*!
    \internal
    \brief Notifies gecko about ambience color scheme change and removes observer.
*/
void SailfishOS::WebEngineSettingsPrivate::oneShotNotifyColorSchemeChanged(const QString &message, const QVariant &data)
{
    Q_UNUSED(data);
    if (message == QLatin1String("embedliteviewcreated")) {
        SailfishOS::WebEngine *webEngine = SailfishOS::WebEngine::instance();
        // Remove the observer and disconnect the signal
        webEngine->removeObserver(QStringLiteral("embedliteviewcreated"));
        disconnect(webEngine, &SailfishOS::WebEngine::recvObserve,
                   this, &SailfishOS::WebEngineSettingsPrivate::oneShotNotifyColorSchemeChanged);

        // Notify about the ambience state
        notifyColorSchemeChanged();
    }
}

/*!
    \brief Returns the instance of the singleton WebEngineSettings class.

    The returned instance may not be intialised.

    \sa initialize
*/
SailfishOS::WebEngineSettings *SailfishOS::WebEngineSettings::instance()
{
    return webEngineSettingsInstance();
}

/*!
    \internal
    \brief Constructs a new WebEngineSettings instance, with the specified QObject \a parent.

    In general there should be no need to use the constructor. Call \l instance
    to return the singleton instance instead.

    \sa instance
*/
SailfishOS::WebEngineSettings::WebEngineSettings(QObject *parent)
    : QMozEngineSettings(parent)
    , d(WebEngineSettingsPrivate::instance())
{
}

/*!
    \internal
    \brief Destroys the WebEngineSettings instance.

    WebEngineSettings is a singleton, so should never be deleted.
*/
SailfishOS::WebEngineSettings::~WebEngineSettings()
{
}
