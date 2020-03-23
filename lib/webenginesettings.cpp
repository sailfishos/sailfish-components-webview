/****************************************************************************
**
** Copyright (c) 2016 - 2020 Jolla Ltd.
** Copyright (c) 2030 Open Mobile Platform LLC.
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "webenginesettings.h"
#include "webengine.h"
#include "opensearchconfigs.h"

#include <silicatheme.h>

#include <QtCore/QDebug>
#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QSize>
#include <QtCore/QtMath>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QStyleHints>

#include <MGConfItem>

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

class SailfishOS::WebEngineSettingsPrivate
{
public:
    MGConfItem searchEngineConfItem { QLatin1String("/apps/sailfish-browser/settings/search_engine") };

    QStringList *addedSearchEngines = nullptr;
    bool searchEnginesInitialized = false;
};

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

    SailfishOS::WebEngine::instance()->addObserver(QLatin1String("embed:search"));

    isInitialized = true;
}

SailfishOS::WebEngineSettings *SailfishOS::WebEngineSettings::instance()
{
    return webEngineSettingsInstance();
}

SailfishOS::WebEngineSettings::WebEngineSettings(QObject *parent)
    : QMozEngineSettings(parent)
    , d_ptr(new WebEngineSettingsPrivate)
{
    connect(&d_ptr->searchEngineConfItem, &MGConfItem::valueChanged,
            this, &WebEngineSettings::setSearchEngine);
    connect(SailfishOS::WebEngine::instance(), &SailfishOS::WebEngine::recvObserve,
            this, &WebEngineSettings::handleObserve);
}

SailfishOS::WebEngineSettings::~WebEngineSettings()
{
}

void SailfishOS::WebEngineSettings::setSearchEngine()
{
    if (d_ptr->searchEnginesInitialized) {
        QVariant searchEngine = d_ptr->searchEngineConfItem.value(QVariant(QString("Google")));

        setPreference(QString("browser.search.defaultenginename"), searchEngine);

        // Let nsSearchService update the search engine (through EmbedLiteSearchEngine).
        QVariantMap defaultSearchEngine;
        defaultSearchEngine.insert(QLatin1String("msg"), QLatin1String("setdefault"));
        defaultSearchEngine.insert(QLatin1String("name"), searchEngine);
        SailfishOS::WebEngine *webEngine = SailfishOS::WebEngine::instance();
        webEngine->notifyObservers(QLatin1String("embedui:search"), QVariant(defaultSearchEngine));
    }
}

void SailfishOS::WebEngineSettings::handleObserve(const QString &message, const QVariant &data)
{
    const QVariantMap dataMap = data.toMap();
    if (message == QLatin1String("embed:search")) {
        QString msg = dataMap.value("msg").toString();
        if (msg == QLatin1String("init")) {
            const QMap<QString, QString> configs(OpenSearchConfigs::getAvailableOpenSearchConfigs());
            const QStringList configuredEngines = configs.keys();
            QStringList registeredSearches(dataMap.value(QLatin1String("engines")).toStringList());
            QString defaultSearchEngine = dataMap.value(QLatin1String("defaultEngine")).toString();
            d_ptr->searchEnginesInitialized = !registeredSearches.isEmpty();

            // Upon first start, engine doesn't know about the search engines.
            // Engine load requests are send within the for loop below.
            if (!d_ptr->searchEnginesInitialized) {
                d_ptr->addedSearchEngines = new QStringList(configuredEngines);
            }

            SailfishOS::WebEngine *webEngine = SailfishOS::WebEngine::instance();

            // Add newly installed configs
            for (QString searchName : configuredEngines) {
                if (registeredSearches.contains(searchName)) {
                    registeredSearches.removeAll(searchName);
                } else {
                    QVariantMap loadsearch;
                    // load opensearch descriptions
                    loadsearch.insert(QLatin1String("msg"), QVariant(QLatin1String("loadxml")));
                    loadsearch.insert(QLatin1String("uri"), QVariant(QString("file://%1").arg(configs[searchName])));
                    loadsearch.insert(QLatin1String("confirm"), QVariant(false));
                    webEngine->notifyObservers(QLatin1String("embedui:search"), QVariant(loadsearch));
                }
            }

            // Remove uninstalled OpenSearch configs
            for (QString searchName : registeredSearches) {
                QVariantMap removeMsg;
                removeMsg.insert(QLatin1String("msg"), QVariant(QLatin1String("remove")));
                removeMsg.insert(QLatin1String("name"), QVariant(searchName));
                webEngine->notifyObservers(QLatin1String("embedui:search"), QVariant(removeMsg));
            }

            // Try to set search engine. After first start we can update the default search
            // engine immediately.
            setSearchEngine();
        } else if (msg == QLatin1String("search-engine-added")) {
            // We're only interrested about the very first start. Then the d_ptr->addedSearchEngines
            // contains engines.
            int errorCode = dataMap.value(QLatin1String("errorCode")).toInt();
            bool firstStart = d_ptr->addedSearchEngines && !d_ptr->addedSearchEngines->isEmpty();
            if (errorCode != 0) {
                qWarning() << "An error occurred while adding a search engine, error code: " << errorCode << ", see nsIBrowserSearchService for more details.";
            } else if (d_ptr->addedSearchEngines) {
                QString engine = dataMap.value(QLatin1String("engine")).toString();
                d_ptr->addedSearchEngines->removeAll(engine);
                d_ptr->searchEnginesInitialized = d_ptr->addedSearchEngines->isEmpty();
                // All engines are added.
                if (firstStart && d_ptr->searchEnginesInitialized) {
                    setSearchEngine();
                    delete d_ptr->addedSearchEngines;
                    d_ptr->addedSearchEngines = 0;
                }
            }
        }
    }
}

