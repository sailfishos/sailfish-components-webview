// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
// SPDX-License-Identifier: MPL-2.0
#include "silicatheme.h"
#include "webengine.h"
#include "webenginesettings.h"
#include <QGuiApplication>
#include <QQmlEngine>
#include <qqml.h>
#include <cassert>

static Silica::Theme *theme;
static int themeCount;

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    qmlRegisterSingletonType<Silica::Theme>("Sailfish.Silica", 1, 0, "Theme",
            [](QQmlEngine *engine, QJSEngine *) -> QObject * {
        ++themeCount;
        // Real Silica imports can re-enter the WebEngine plugin.
        SailfishOS::WebEngineSettings::initialize();
        theme = new Silica::Theme(engine);
        QQmlEngine::setObjectOwnership(theme, QQmlEngine::CppOwnership);
        return theme;
    });
    auto engine = SailfishOS::WebEngine::instance();
    auto settings = SailfishOS::WebEngineSettings::instance();
    SailfishOS::WebEngine::initialize("/tmp/theme-profile", false);
    QString scheme;
    engine->addObserver("ambience-theme-changed");
    QObject::connect(engine, &SailfishOS::WebEngine::recvObserve,
                     [&](const QString &topic, const QVariant &data) {
        if (topic == "ambience-theme-changed") scheme = data.toString();
    });
    const bool late = app.arguments().contains("--late");
    if (late) engine->runEmbedding();
    SailfishOS::WebEngineSettings::initialize();
    assert(themeCount == 1 && theme);
    // Native callers receive defaults before initialize() returns.
    assert(settings->pixelRatio() == 1.5);
    if (!late) engine->runEmbedding();
    assert(scheme == "dark");
    theme->setColorScheme(Silica::Theme::DarkOnLight);
    assert(scheme == "light");
    theme->setColorScheme(Silica::Theme::LightOnDark);
    assert(scheme == "dark");
    scheme.clear();
    engine->notifyObservers("embedliteviewcreated", QVariant());
    assert(scheme == "dark");
    // A no-op explicit setter is safe too: there is no delayed default write.
    settings->setPixelRatio(1.5);
    QCoreApplication::processEvents();
    assert(settings->pixelRatio() == 1.5);
    settings->setPixelRatio(1.0);
    {
        QQmlEngine first;
        SailfishOS::WebEngineSettings::initialize();
        QQmlEngine second;
        SailfishOS::WebEngineSettings::initialize();
    }
    QCoreApplication::processEvents();
    assert(settings->pixelRatio() == 1.0 && themeCount == 1);
    QMozEngineSettings::instance()->setPixelRatio(2.5);
    SailfishOS::WebEngineSettings::initialize();
    assert(settings->pixelRatio() == 2.5);
    engine->stopEmbedding();
}
