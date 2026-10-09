// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
// SPDX-License-Identifier: MPL-2.0
#include "webengine.h"
#include "webenginesettings.h"
#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QtTest>

static void importWebEngine(QQmlEngine *engine)
{
    QQmlComponent component(engine);
    component.setData("import QtQuick 2.6\nimport Sailfish.WebEngine 1.0\nQtObject { property real ratio: WebEngineSettings.pixelRatio }",
                      QUrl());
    QScopedPointer<QObject> object(component.create());
    if (!object) qFatal("WebEngine import failed: %s", qPrintable(component.errorString()));
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("tst_themeinitialization");
    QStandardPaths::setTestModeEnabled(true);
    QTemporaryDir profile;
    if (!profile.isValid()) return 1;
    SailfishOS::WebEngine::initialize(profile.path(), false);
    auto settings = SailfishOS::WebEngineSettings::instance();
    if (app.arguments().contains("--qml-first")) {
        QQmlEngine first;
        importWebEngine(&first);
    } else {
        SailfishOS::WebEngineSettings::initialize();
    }
    const qreal initialRatio = settings->pixelRatio();
    if (initialRatio < 1.5) qFatal("Native theme defaults not initialized");
    settings->setPixelRatio(initialRatio);
    QCoreApplication::processEvents();
    if (settings->pixelRatio() != initialRatio) qFatal("No-op override changed");
    settings->setPixelRatio(1.0);
    {
        QQmlEngine second;
        importWebEngine(&second);
        SailfishOS::WebEngineSettings::initialize();
    }
    QCoreApplication::processEvents();
    if (settings->pixelRatio() != 1.0) qFatal("Override changed on repeat import");
    qInfo() << "PASS: theme initialization, ratio" << initialRatio
            << "and explicit overrides";
    return 0;
}
