TARGET = tst_downloadhelper

include(../test_common.pri)

QT -= gui

CONFIG += link_pkgconfig
PKGCONFIG += qt5embedwidget

target.path = /opt/tests/sailfish-components-webview/auto
INSTALLS += target

INCLUDEPATH += ../../../lib
LIBS += -L../../../lib -lsailfishwebengine

SOURCES += tst_downloadhelper.cpp
