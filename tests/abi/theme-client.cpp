// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
// SPDX-License-Identifier: MPL-2.0
#include "../../import/theme/themeadapter.h"
#include "silicatheme.h"
#include "webengine.h"
#include <QGuiApplication>
#include <cassert>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    qmlRegisterSingletonType<Silica::Theme>("Sailfish.Silica", 1, 0, "Theme",
            [](QQmlEngine *engine, QJSEngine *) -> QObject * {
        auto theme = new Silica::Theme(engine);
        QQmlEngine::setObjectOwnership(theme, QQmlEngine::CppOwnership);
        return theme;
    });
    auto engine = SailfishOS::WebEngine::instance();
    auto settings = SailfishOS::WebEngineSettings::instance();
    SailfishOS::WebEngine::initialize("/tmp/theme-profile", false);
    SailfishOS::WebEngineSettings::initialize();
    QString scheme;
    engine->addObserver("ambience-theme-changed");
    QObject::connect(engine, &SailfishOS::WebEngine::recvObserve,
                     [&](const QString &topic, const QVariant &data) {
        if (topic == "ambience-theme-changed") scheme = data.toString();
    });
    const bool late = argc > 2;
    if (late) engine->runEmbedding();
    QQmlEngine qml;
    const QUrl url = QUrl::fromLocalFile(QString::fromLocal8Bit(argv[1]));
    SailfishOS::initializeWebEngineTheme(&qml, url);
    assert(qml.property("_sailfish_webengine_theme").toBool());
    assert(settings->pixelRatio() == 1.5);
    if (!late) engine->runEmbedding();
    assert(scheme == "dark");
    qml.findChild<Silica::Theme *>()->setColorScheme(Silica::Theme::DarkOnLight);
    assert(scheme == "light");
    qml.findChild<Silica::Theme *>()->setColorScheme(Silica::Theme::LightOnDark);
    assert(scheme == "dark");
    scheme.clear();
    engine->notifyObservers("embedliteviewcreated", QVariant());
    assert(scheme == "dark");
    // Repeated imports and additional QML engines must preserve application overrides.
    settings->setPixelRatio(2.5);
    SailfishOS::initializeWebEngineTheme(&qml, url);
    QQmlEngine second;
    SailfishOS::initializeWebEngineTheme(&second, url);
    assert(settings->pixelRatio() == 2.5);
    engine->stopEmbedding();
}
