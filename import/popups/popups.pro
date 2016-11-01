TEMPLATE=lib

TARGET=sailfishwebviewpopupsplugin
TARGET = $$qtLibraryTarget($$TARGET)

MODULENAME = Sailfish/WebView/Popups
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

include(../../defaults.pri)

CONFIG += plugin
QT += qml quick

QMAKE_CXXFLAGS += -fPIC

HEADERS += popupsplugin.h
SOURCES += popupsplugin.cpp
OTHER_FILES += qmldir *qml

include(popupstranslations.pri)

import.files = qmldir *.qml
import.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target import translations_install engineering_english_install
