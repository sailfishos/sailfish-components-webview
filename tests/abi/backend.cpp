// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
// SPDX-License-Identifier: MPL-2.0
#include "backend.h"
#include "runtime/qmozruntime_p.h"
#include "runtime/qmozchromewindowregistry_p.h"
#include "qmozcontext.h"

using namespace mozilla::embedlite;
QMozRuntime::QMozRuntime(EmbedLiteAppListener *listener, QObject *parent)
    : QObject(parent), mApp(new EmbedLiteApp), mQtPump(nullptr), mEmbedStarted(false)
{ mApp->listener = listener; }
QMozRuntime::~QMozRuntime() { delete mApp; }
EmbedLiteApp *QMozRuntime::embedLiteApp() const { return mApp; }
EmbedLiteMessagePump *QMozRuntime::embedLoop() const { return nullptr; }
bool QMozRuntime::hasApp() const { return mApp; }
void QMozRuntime::start() {
    if (!mEmbedStarted) {
        mEmbedStarted = true;
        qApp->setProperty("abi.starts", qApp->property("abi.starts").toInt() + 1);
        mApp->listener->Initialized();
    }
}
void QMozRuntime::stop() {
    qApp->setProperty("abi.stops", qApp->property("abi.stops").toInt() + 1);
    if (mApp->listener) mApp->listener->Destroyed();
}
void QMozRuntime::detachListener() { mApp->listener = nullptr; }
void QMozRuntime::backendDestroyed() {}
namespace QtMoz {
bool hasTrackedChromeWindows(QMozContext *) { return false; }
bool releaseTrackedChromeWindows(QMozContext *) { return false; }
}

// Test driver for the real context callback, including reentrant window creation.
extern "C" void abiLastWindowDestroyed() {
    EmbedLiteApp *app = QMozContext::instance()->GetApp();
    app->windows = 0;
    app->listener->LastWindowDestroyed();
}
extern "C" void abiCreateWindow() {
    QMozContext::instance()->GetApp()->windows++;
}
