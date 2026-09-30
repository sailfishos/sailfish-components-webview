// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
// SPDX-License-Identifier: MPL-2.0
// Compiled only against the pinned, pre-ESR153 public headers.
#include <string>
#include <vector>
#include <webengine.h>
#include <webenginesettings.h>
#include <QGuiApplication>
#include <QMetaEnum>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QUrl>
#include <QVariantMap>
#include <cassert>
#include <dlfcn.h>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    auto engine = SailfishOS::WebEngine::instance();
    auto settings = SailfishOS::WebEngineSettings::instance();
    assert(qobject_cast<QMozContext *>(engine) == engine);
    assert(qobject_cast<QMozEngineSettings *>(settings) == settings);
    assert(engine->metaObject()->superClass() == &QMozContext::staticMetaObject);
    assert(settings->metaObject()->superClass() == &QMozEngineSettings::staticMetaObject);
    assert(engine->inherits("QMozContext"));
    assert(settings->inherits("QMozEngineSettings"));
    auto engineSize = reinterpret_cast<size_t (*)()>(dlsym(RTLD_DEFAULT, "abiEngineSize"));
    auto settingsSize = reinterpret_cast<size_t (*)()>(dlsym(RTLD_DEFAULT, "abiSettingsSize"));
    assert(engineSize && engineSize() == sizeof(SailfishOS::WebEngine));
    assert(settingsSize && settingsSize() == sizeof(SailfishOS::WebEngineSettings));
    const int enumIndex = settings->metaObject()->indexOfEnumerator("CookieBehavior");
    assert(enumIndex >= 0);
    assert(settings->metaObject()->enumerator(enumIndex).keyToValue("BlockAll")
           == SailfishOS::WebEngineSettings::BlockAll);

    int initialized = 0;
    int destroyed = 0;
    QObject::connect(engine, &SailfishOS::WebEngine::initialized,
                     [&]() { ++initialized; });
    QObject::connect(engine, &SailfishOS::WebEngine::contextDestroyed,
                     [&]() { ++destroyed; });
    const bool deferred = argc > 1;
    SailfishOS::WebEngine::initialize("/tmp/legacy-profile", deferred);
    assert(!engine->isInitialized());
    const QUrl sheet(QStringLiteral("file:///tmp/compat.css"));
    assert(QMetaObject::invokeMethod(engine, "addUserStyleSheet", Q_ARG(QUrl, sheet)));
    assert(!app.property("file:///tmp/compat.css").isValid());
    engine->setProfile("/tmp/application-profile");
    engine->addComponentManifest("/tmp/application.manifest");
    assert(app.property("abi.profile").toString() == "/tmp/application-profile");
    const QStringList manifests = app.property("abi.manifests").toStringList();
    assert(manifests.size() == 5 && manifests.last() == "/tmp/application.manifest");
    SailfishOS::WebEngine::initialize("/tmp/ignored-profile", true);
    assert(!engine->isInitialized());
    if (deferred) QCoreApplication::processEvents();
    else engine->runEmbedding();
    assert(initialized == 1 && engine->isInitialized());
    assert(app.property("file:///tmp/compat.css").toBool());
    assert(QMetaObject::invokeMethod(engine, "removeUserStyleSheet", Q_ARG(QUrl, sheet)));
    assert(!app.property("file:///tmp/compat.css").toBool());
    engine->runEmbedding();
    assert(app.property("abi.starts").toInt() == 1);
    assert(QMozContext::instance()->isInitialized());

    int received = 0;
    QVariant lastData;
    QObject::connect(engine, &SailfishOS::WebEngine::recvObserve,
                     [&](const QString &topic, const QVariant &data) {
        if (topic == "abi-topic") { ++received; lastData = data; }
    });
    engine->addObserver("abi-topic");
    engine->addObservers({"abi-topic"});
    engine->notifyObservers("abi-topic", QStringLiteral("plain"));
    assert(received == 1 && lastData.toString() == "plain");
    engine->removeObserver("abi-topic");
    engine->notifyObservers("abi-topic", QVariant(QVariantMap{{"number", 42}}));
    assert(received == 2 && lastData.toMap().value("number").toInt() == 42);
    engine->removeObservers({"abi-topic"});
    engine->notifyObservers("abi-topic", QStringLiteral("ignored"));
    assert(received == 2);

    SailfishOS::WebEngineSettings::initialize();
    int imagesChanged = 0;
    QObject::connect(settings, &SailfishOS::WebEngineSettings::autoLoadImagesChanged,
                     [&]() { ++imagesChanged; });
    settings->setAutoLoadImages(false);
    assert(!settings->autoLoadImages() && imagesChanged == 1);
    assert(!QMozEngineSettings::instance()->autoLoadImages());
    settings->setJavascriptEnabled(false);
    assert(!settings->javascriptEnabled());
    settings->setPopupEnabled(true);
    assert(settings->popupEnabled());
    settings->setCookieBehavior(SailfishOS::WebEngineSettings::BlockAll);
    assert(settings->cookieBehavior() == SailfishOS::WebEngineSettings::BlockAll);
    settings->setUseDownloadDir(true);
    assert(settings->useDownloadDir());
    settings->setDownloadDir("/tmp/downloads");
    assert(settings->downloadDir() == "/tmp/downloads");
    settings->setPixelRatio(2.0);
    assert(settings->pixelRatio() == 2.0);
    settings->setDoNotTrack(true);
    assert(settings->doNotTrack());
    settings->setColorScheme(SailfishOS::WebEngineSettings::PrefersDarkMode);
    assert(settings->colorScheme() == SailfishOS::WebEngineSettings::PrefersDarkMode);
    settings->setPreference("abi.int", 12, SailfishOS::WebEngineSettings::IntPref);
    settings->setPreference("abi.bool", true);
    assert(app.property("abi.int").toInt() == 12 && app.property("abi.bool").toBool());
    settings->setTileSize(QSize(256, 256));
    settings->enableProgressivePainting(true);
    settings->enableLowPrecisionBuffers(true);
    assert(app.property("layers.progressive-paint").toBool());
    assert(app.property("layers.low-precision-buffer").toBool());
    assert(settings->isInitialized());

    qmlRegisterSingletonType<SailfishOS::WebEngine>("Legacy.WebEngine", 1, 0, "WebEngine",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            auto value = SailfishOS::WebEngine::instance();
            QQmlEngine::setObjectOwnership(value, QQmlEngine::CppOwnership);
            return value;
        });
    qmlRegisterSingletonType<SailfishOS::WebEngineSettings>("Legacy.WebEngine", 1, 0, "WebEngineSettings",
        [](QQmlEngine *, QJSEngine *) -> QObject * {
            auto value = SailfishOS::WebEngineSettings::instance();
            QQmlEngine::setObjectOwnership(value, QQmlEngine::CppOwnership);
            return value;
        });
    QQmlEngine qml;
    QQmlComponent component(&qml);
    component.setData("import QtQml 2.0\nimport Legacy.WebEngine 1.0\n"
        "QtObject { property int cookie: WebEngineSettings.BlockAll; "
        "property bool ready: WebEngine.initialized; "
        "property bool images: WebEngineSettings.autoLoadImages }", QUrl());
    QScopedPointer<QObject> object(component.create());
    if (!object) qWarning() << component.errors();
    assert(object && object->property("cookie").toInt() == 2);
    assert(object->property("ready").toBool());
    assert(!object->property("images").toBool());
    settings->setAutoLoadImages(true);
    assert(object->property("images").toBool());

    auto lastWindow = reinterpret_cast<void (*)()>(dlsym(RTLD_DEFAULT, "abiLastWindowDestroyed"));
    auto createWindow = reinterpret_cast<void (*)()>(dlsym(RTLD_DEFAULT, "abiCreateWindow"));
    assert(lastWindow && createWindow);
    QString lifecycle;
    bool reopen = true;
    QObject::connect(engine, &SailfishOS::WebEngine::lastViewDestroyed, [&]() {
        lifecycle += 'v';
        if (reopen) createWindow();
    });
    QObject::connect(engine, &SailfishOS::WebEngine::lastWindowDestroyed,
                     [&]() { lifecycle += 'w'; });
    lastWindow();
    assert(lifecycle == "v");
    reopen = false;
    lastWindow();
    assert(lifecycle == "vvw");
    engine->stopEmbedding();
    assert(destroyed == 1 && app.property("abi.stops").toInt() == 1);
}
