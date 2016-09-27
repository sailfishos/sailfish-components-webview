TEMPLATE=lib

TARGET=sailfishwebviewplugin
TARGET = $$qtLibraryTarget($$TARGET)

MODULENAME = Sailfish/WebView
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

include(../../defaults.pri)

CONFIG += link_pkgconfig plugin
QT += quick quick-private
PKGCONFIG += qt5embedwidget

QMAKE_CXXFLAGS += -fPIC

INCLUDEPATH += . src ../../lib
LIBS += -L../../lib -lsailfishwebengine

HEADERS += plugin.h \
            rawwebview.h
SOURCES += plugin.cpp \
            rawwebview.cpp
OTHER_FILES += qmldir *.qml

include(translations.pri)

import.files = qmldir *.qml
import.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target import translations_install engineering_english_install
