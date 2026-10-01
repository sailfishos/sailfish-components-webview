/****************************************************************************
**
** Copyright (c) 2016 - 2021 Jolla Ltd.
** Copyright (c) 2024 - 2026 Jolla Mobile Ltd
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "webengine.h"

#include <QCoreApplication>
#include <QTimer>
#include <qmozcontext.h>

Q_GLOBAL_STATIC(SailfishOS::WebEngine, webEngineInstance)

namespace SailfishOS {

namespace {
const char UserStyleSheetsProperty[] = "_sailfishWebEngineUserStyleSheets";
}

void WebEngine::initialize(const QString &profilePath, bool runEmbedding)
{
    static bool isInitialized = false;
    if (isInitialized) {
        return;
    }

    // Workaround for https://bugzilla.mozilla.org/show_bug.cgi?id=929879
    setenv("LC_NUMERIC", "C", 1);
    setlocale(LC_NUMERIC, "C");

    setenv("USE_NEMO_GSTREAMER", "1", 1);
    setenv("NO_LIMIT_ONE_GST_DECODER", "1", 1);

    setenv("PULSE_PROP_application.process.binary",
           qApp->applicationName().toUtf8(), 1);

    WebEngine *webEngine = instance();
    QMozContext *context = webEngine;
    context->setProfile(profilePath);

    // Register the default manifests before applications add their own.
    const QString componentsPath =
            QStringLiteral(SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH);
    context->addComponentManifest(
            componentsPath + QStringLiteral("/components/EmbedLiteBinComponents.manifest"));
    context->addComponentManifest(
            componentsPath + QStringLiteral("/components/EmbedLiteJSComponents.manifest"));
    context->addComponentManifest(
            componentsPath + QStringLiteral("/chrome/EmbedLiteJSScripts.manifest"));
    context->addComponentManifest(
            componentsPath + QStringLiteral("/chrome/EmbedLiteOverrides.manifest"));

    if (runEmbedding) {
        QTimer::singleShot(0, webEngine, [webEngine]() {
            webEngine->runEmbedding();
        });
    }

    isInitialized = true;
}

WebEngine *WebEngine::instance()
{
    return webEngineInstance();
}

WebEngine::WebEngine(QObject *parent)
    : QMozContext(parent)
{
    // Keep WebEngine's original size and QMozContext base. Per-instance
    // additions belong in QObject storage, not in new public data members.
    connect(this, &WebEngine::initialized, this, [this]() {
        const QStringList sheets = property(UserStyleSheetsProperty).toStringList();
        for (const QString &uri : sheets) {
            loadUserStyleSheet(uri);
        }
    });
}

WebEngine::~WebEngine()
{
}

void WebEngine::addUserStyleSheet(const QUrl &url)
{
    const QString uri = url.toString();
    QStringList sheets = property(UserStyleSheetsProperty).toStringList();
    if (uri.isEmpty() || sheets.contains(uri)) {
        return;
    }
    sheets.append(uri);
    setProperty(UserStyleSheetsProperty, sheets);
    if (isInitialized()) {
        loadUserStyleSheet(uri);
    }
}

void WebEngine::removeUserStyleSheet(const QUrl &url)
{
    const QString uri = url.toString();
    QStringList sheets = property(UserStyleSheetsProperty).toStringList();
    if (!sheets.removeOne(uri)) {
        return;
    }
    setProperty(UserStyleSheetsProperty, sheets);
    if (isInitialized()) {
        loadUserStyleSheet(uri, false);
    }
}

} // namespace SailfishOS
