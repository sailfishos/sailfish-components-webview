// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
// SPDX-License-Identifier: MPL-2.0

#ifndef SAILFISHOS_WEBVIEW_THEMEADAPTER_H
#define SAILFISHOS_WEBVIEW_THEMEADAPTER_H

#include "webenginesettings_p.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>

namespace SailfishOS {

inline void initializeWebEngineTheme(QQmlEngine *engine,
        const QUrl &themeUrl = QUrl::fromLocalFile(
                QStringLiteral("/usr/share/sailfish-webview/WebEngineTheme.qml")))
{
    // Both imports can be used in the same engine. Keep one theme observer.
    if (engine->property("_sailfish_webengine_theme").toBool()) {
        return;
    }

    QQmlContext *context = new QQmlContext(engine->rootContext(), engine);
    context->setContextProperty(QStringLiteral("webEngineThemeSettings"),
                                WebEngineSettingsPrivate::instance());
    QQmlComponent component(engine, themeUrl);
    QObject *theme = component.create(context);
    if (!theme) {
        qWarning() << "Could not initialize WebEngine theme:" << component.errors();
        delete context;
        return;
    }
    theme->setParent(context);
    engine->setProperty("_sailfish_webengine_theme", true);
}

}

#endif
