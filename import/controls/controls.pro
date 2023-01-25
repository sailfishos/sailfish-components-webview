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
    plugins.qmltypes \
    files(*.qml)

include(controlstranslations.pri)

import.files = qmldir plugins.qmltypes *.qml *.js
import.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target import translations_install engineering_english_install

# The module is verbose on stdout, hence the use of dedicated FD for output.
#
# HACK: pass -nocomposites to work around the issue with types leaked
# (mostly) from Silica. All of these are composite types and we have no other
# composite types.
qmltypes.commands = qmlplugindump -noinstantiate -nonrelocatable -nocomposites \
         Sailfish.WebView.Controls 1.0 --output-fd 3 3> $$PWD/plugins.qmltypes \
    || true # it often segfaults on exit
QMAKE_EXTRA_TARGETS += qmltypes
