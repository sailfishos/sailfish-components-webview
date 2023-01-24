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
OTHER_FILES += qmldir plugins.qmltypes

import.files = qmldir plugins.qmltypes
import.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target import

# The module is verbose on stdout, hence the use of dedicated FD for output.
qmltypes.commands = qmlplugindump -noinstantiate -nonrelocatable \
         Sailfish.WebEngine 1.0 --output-fd 3 3> $$PWD/plugins.qmltypes \
    || true # it often segfaults on exit
QMAKE_EXTRA_TARGETS += qmltypes
