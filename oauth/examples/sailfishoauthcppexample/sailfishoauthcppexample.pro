TEMPLATE = app
TARGET = sailfishoauthcppexample
TARGETPATH = /usr/bin/
target.path = $$TARGETPATH

CONFIG += qt
QT += gui network

# normally, clients would do the following:
#CONFIG += link_pkgconfig
#PKGCONFIG += sailfishoauth

# but we are building this example in tree, so instead we do:
INCLUDEPATH += . ../../lib
LIBS += -L../../lib -lsailfishoauth

SOURCES += main.cpp
INSTALLS += target
