TEMPLATE=lib

TARGET=sailfishwebviewcontrolsplugin
TARGET = $$qtLibraryTarget($$TARGET)

MODULENAME = Sailfish/WebView/Controls
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

include(../../defaults.pri)

CONFIG += plugin
QT += qml quick

QMAKE_CXXFLAGS += -fPIC

SOURCES += \
    controlsplugin.cpp
OTHER_FILES += \
    qmldir \
    files(*.qml)

include(controlstranslations.pri)

import.files = qmldir *.qml *.js
import.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target import translations_install engineering_english_install
