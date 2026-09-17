TEMPLATE = app
TARGET = custompopups
TARGETPATH = /usr/bin/
target.path = $$TARGETPATH
QT += gui qml quick
QMAKE_CXXFLAGS += -fPIE

SOURCES += main.cpp
RESOURCES += resources.qrc

INSTALLS += target
