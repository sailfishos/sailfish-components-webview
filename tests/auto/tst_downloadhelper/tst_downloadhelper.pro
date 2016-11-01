TARGET = tst_downloadhelper

include(../test_common.pri)

QT -= gui

target.path = /opt/tests/sailfish-components-webview/auto
INSTALLS += target

INCLUDEPATH += ../../../lib
LIBS += -L../../../lib -lsailfishwebengine

SOURCES += tst_downloadhelper.cpp
