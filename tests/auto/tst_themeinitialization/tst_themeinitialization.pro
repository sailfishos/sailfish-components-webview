TARGET = tst_themeinitialization
include(../test_common.pri)
QT += gui qml
CONFIG += console link_pkgconfig
CONFIG -= app_bundle
INCLUDEPATH += ../../../lib
LIBS += -L../../../lib -lsailfishwebengine
PKGCONFIG += qt5embedwidget
SOURCES += tst_themeinitialization.cpp
target.path = /opt/tests/sailfish-components-webview/auto
INSTALLS += target
