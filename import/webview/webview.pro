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
OTHER_FILES += qmldir plugins.qmltypes *.qml

include(translations.pri)

import.files = qmldir plugins.qmltypes *.qml
import.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target import translations_install engineering_english_install

# The module is verbose on stdout, hence the use of dedicated FD for output.
#
# HACK: pass -nocomposites to work around the issue with types leaked
# (mostly) from Silica. All of these are composite types and we have no other
# composite types.
qmltypes.commands = qmlplugindump -noinstantiate -nonrelocatable -nocomposites \
         Sailfish.WebView 1.0 --output-fd 3 3>$$PWD/plugins.qmltypes \
    || true # it often segfaults on exit
QMAKE_EXTRA_TARGETS += qmltypes
