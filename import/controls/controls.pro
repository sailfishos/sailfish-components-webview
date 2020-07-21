TEMPLATE=lib

TARGET=sailfishwebviewcontrolsplugin
TARGET = $$qtLibraryTarget($$TARGET)

MODULENAME = Sailfish/WebView/Controls
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

include(../../defaults.pri)

CONFIG += plugin
QT += qml quick

QMAKE_CXXFLAGS += -fPIC

INCLUDEPATH += . src ../../lib
LIBS += -L../../lib -lsailfishwebengine

HEADERS += \
    permissionmanager.h \
    permissionmodel.h \
    permissionfilterproxymodel.h

SOURCES += \
    controlsplugin.cpp \
    permissionmanager.cpp \
    permissionmodel.cpp \
    permissionfilterproxymodel.cpp

OTHER_FILES += \
    qmldir \
    files(*.qml)

include(controlstranslations.pri)

import.files = qmldir *.qml *.js
import.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target import translations_install engineering_english_install
