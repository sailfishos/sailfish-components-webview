TEMPLATE=lib

TARGET=sailfishwebviewpickersplugin
TARGET = $$qtLibraryTarget($$TARGET)

MODULENAME = Sailfish/WebView/Pickers
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

CONFIG += qt plugin
QT += core qml quick

QMAKE_CXXFLAGS += -fPIC

HEADERS += pickersplugin.h
SOURCES += pickersplugin.cpp
OTHER_FILES += qmldir *qml

include(pickerstranslations.pri)

import.files = qmldir *.qml
import.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target import translations_install engineering_english_install
