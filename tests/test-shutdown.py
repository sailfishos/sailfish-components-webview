#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
# SPDX-License-Identifier: MPL-2.0
"""Exercise the production close-event controller with a controllable engine."""
from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[1]
source = (root / 'import/webview/plugin.cpp').read_text()
controller = source[source.index('QEvent::Type shutdownEventType()'):source.index('\nWebViewShutdownController *shutdownController(')]
# Access the watchdog to trigger its real timeout handler without sleeping.
controller = controller.replace('private:', 'public:')
header = '''#include <QObject>
namespace SailfishOS {
class WebEngine : public QObject {
 Q_OBJECT
public:
 int stops = 0;
 bool finishImmediately = false;
 void stopEmbedding() { ++stops; if (finishImmediately) emit contextDestroyed(); }
signals:
 void contextDestroyed();
 void lastWindowDestroyed();
};
}
class QMozContext {
public:
 static QMozContext *instance() { static QMozContext c; return &c; }
 int getNumberOfWindows() const { return 0; }
};
class RawWebView {
public:
 static bool hasLiveViews() { return false; }
 static void destroyLiveViews() {}
};
'''
main = r'''
int main(int argc, char **argv) {
 QGuiApplication app(argc, argv);
 app.setQuitOnLastWindowClosed(false);
 for (int scenario = 0; scenario < 3; ++scenario) {
  SailfishOS::WebEngine engine;
  // Complete cleanup after observing the controller's explicit quit.
  auto cleanup = QObject::connect(&app, &QCoreApplication::aboutToQuit, &engine,
                                  [&]() { emit engine.contextDestroyed(); });
  WebViewShutdownController c(&engine, nullptr);
  QWindow window;
  QTimer deadline;
  deadline.setSingleShot(true);
  QObject::connect(&deadline, &QTimer::timeout, []() { QCoreApplication::exit(9); });
  QTimer::singleShot(0, [&]() {
    if (scenario == 0) engine.finishImmediately = true;
    if (scenario == 2) {
      c.scheduleShutdown(false);
      QCoreApplication::sendPostedEvents(&c, shutdownEventType());
      assert(engine.stops == 1 && !c.m_quitAfterShutdown);
    }
    QCloseEvent event;
    assert(c.eventFilter(&window, &event));
    assert(c.m_quitAfterShutdown);
    QCoreApplication::sendPostedEvents(&c, shutdownEventType());
    assert(engine.stops == 1);
    if (scenario != 0) {
      assert(c.m_shutdownWatchdog.isActive());
      QMetaObject::invokeMethod(&c.m_shutdownWatchdog, "timeout", Qt::DirectConnection);
      assert(c.m_shutdownTimedOut);
      QCloseEvent retry;
      assert(!c.eventFilter(&window, &retry));
    }
  });
  deadline.start(2000);
  assert(app.exec() == 0);
  assert(c.m_contextDestroyed);
  QCloseEvent after;
  assert(!c.eventFilter(&window, &after));
  QObject::disconnect(cleanup);
 }
}
'''
with tempfile.TemporaryDirectory(prefix='webview-shutdown-') as directory:
    build = Path(directory)
    (build / 'stubs.h').write_text(header)
    (build / 'test.cpp').write_text('''#include <QGuiApplication>
#include <QWindow>
#include <QCloseEvent>
#include <QEventLoop>
#include <QPointer>
#include <QTimer>
#include <QQmlEngine>
#include <QDebug>
#include <cassert>
#include "stubs.h"
''' + controller + main)
    (build / 'test.pro').write_text('''TEMPLATE = app
TARGET = test
QT += gui qml
CONFIG += console c++11 debug
CONFIG -= app_bundle release
SOURCES += test.cpp
HEADERS += stubs.h
''')
    subprocess.run(['qmake', 'test.pro'], cwd=build, check=True, stdout=subprocess.DEVNULL)
    subprocess.run(['make', '-j2'], cwd=build, check=True, stdout=subprocess.DEVNULL)
    subprocess.run([str(build / 'test')], check=True, timeout=15,
                   env={**os.environ, 'QT_QPA_PLATFORM': 'offscreen'})
print('Successful, timed-out and engine-first shutdown tests passed')
