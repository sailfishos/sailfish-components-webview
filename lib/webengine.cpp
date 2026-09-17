/****************************************************************************
**
** Copyright (C) 2016-2026 Jolla Mobile Ltd
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "webengine.h"
#include "webengine_p.h"

#include <QCoreApplication>
#include <qmozcontext.h>

Q_GLOBAL_STATIC(SailfishOS::WebEngine, webEngineInstance)

namespace SailfishOS {

WebEnginePrivate::WebEnginePrivate()
    : context(QMozContext::instance())
{
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
    QMozContext *context = webEngine->d->context;
    context->setProfile(profilePath);

    // These manifests are an implementation detail of WebView. Applications
    // configure content with semantic WebView APIs instead.
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
        webEngine->runEmbedding();
    }

    isInitialized = true;
}

WebEngine *WebEngine::instance()
{
    return webEngineInstance();
}

WebEngine::WebEngine(QObject *parent)
    : QObject(parent)
    , d(new WebEnginePrivate)
{
    connect(d->context, &QMozContext::initialized,
            this, &WebEngine::initialized);
    connect(d->context, &QMozContext::contextDestroyed,
            this, &WebEngine::contextDestroyed);
    connect(d->context, &QMozContext::lastWindowDestroyed,
            this, &WebEngine::lastWindowDestroyed);
    connect(d->context, &QMozContext::recvObserve,
            this, &WebEngine::recvObserve);
    connect(this, &WebEngine::initialized, this, [this]() {
        Q_FOREACH (const QString &uri, d->userStyleSheets) {
            d->context->loadUserStyleSheet(uri);
        }
    });
}

WebEngine::~WebEngine()
{
    delete d;
}

bool WebEngine::isInitialized() const
{
    return d->context->isInitialized();
}

bool WebEngine::isAccelerated() const
{
    return d->context->isAccelerated();
}

WebEngine::TaskHandle WebEngine::PostUITask(
        TaskCallback callback, void *data, int timeout)
{
    return d->context->PostUITask(callback, data, timeout);
}

WebEngine::TaskHandle WebEngine::PostCompositorTask(
        TaskCallback callback, void *data, int timeout)
{
    return d->context->PostCompositorTask(callback, data, timeout);
}

void WebEngine::CancelTask(TaskHandle handle)
{
    d->context->CancelTask(handle);
}

void WebEngine::addObservers(const std::vector<std::string> &observers)
{
    d->context->addObservers(observers);
}

void WebEngine::removeObservers(const std::vector<std::string> &observers)
{
    d->context->removeObservers(observers);
}

int WebEngine::getNumberOfWindows() const
{
    return d->context->getNumberOfWindows();
}

void WebEngine::setIsAccelerated(bool accelerated)
{
    d->context->setIsAccelerated(accelerated);
}

void WebEngine::addObserver(const QString &topic)
{
    d->context->addObserver(topic);
}

void WebEngine::removeObserver(const QString &topic)
{
    d->context->removeObserver(topic);
}

void WebEngine::notifyObservers(const QString &topic, const QString &value)
{
    d->context->notifyObservers(topic, value);
}

void WebEngine::notifyObservers(const QString &topic, const QVariant &value)
{
    d->context->notifyObservers(topic, value);
}

void WebEngine::addUserStyleSheet(const QUrl &url)
{
    const QString uri = url.toString();
    if (uri.isEmpty() || d->userStyleSheets.contains(uri)) {
        return;
    }
    d->userStyleSheets.append(uri);
    if (isInitialized()) {
        d->context->loadUserStyleSheet(uri);
    }
}

void WebEngine::removeUserStyleSheet(const QUrl &url)
{
    const QString uri = url.toString();
    if (!d->userStyleSheets.removeOne(uri) || !isInitialized()) {
        return;
    }
    d->context->loadUserStyleSheet(uri, false);
}

void WebEngine::runEmbedding(int delay)
{
    d->context->runEmbedding(delay);
}

void WebEngine::stopEmbedding()
{
    d->context->stopEmbedding();
}

void WebEngine::notifyFirstUIInitialized()
{
    d->context->notifyFirstUIInitialized();
}

} // namespace SailfishOS
