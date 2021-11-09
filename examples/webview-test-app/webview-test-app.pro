TEMPLATE=app
TARGET=webview-test-app
TARGETPATH = /usr/bin/
target.path = $$TARGETPATH
QT += gui qml quick
QMAKE_CXXFLAGS += -fPIE

CONFIG += link_pkgconfig
PKGCONFIG += qt5embedwidget

SOURCES += main.cpp
RESOURCES += resources.qrc

INSTALLS += target
