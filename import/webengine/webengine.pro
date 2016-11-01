TEMPLATE=lib

TARGET=sailfishwebengineplugin
TARGET = $$qtLibraryTarget($$TARGET)

MODULENAME = Sailfish/WebEngine
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

include(../../defaults.pri)

CONFIG += link_pkgconfig plugin
QT += qml
PKGCONFIG += qt5embedwidget

QMAKE_CXXFLAGS += -fPIC

INCLUDEPATH += . src ../../lib
LIBS += -L../../lib -lsailfishwebengine

SOURCES += plugin.cpp
OTHER_FILES += qmldir

import.files = qmldir
import.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target import
