TEMPLATE=lib

TARGET=sailfishoauthplugin
TARGET = $$qtLibraryTarget($$TARGET)

MODULENAME = Sailfish/OAuth
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

include(../../defaults.pri)

CONFIG += plugin
QT += qml network

QMAKE_CXXFLAGS += -fPIC

INCLUDEPATH += . ../lib
LIBS += -L../lib -lsailfishoauth

HEADERS += plugin.h
SOURCES += plugin.cpp
OTHER_FILES += qmldir *.qml *.js

include(translations.pri)

import.files = qmldir *.qml *.js
import.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target import translations_install engineering_english_install
