TEMPLATE = lib
TARGET = sailfishwebengine
TARGET = $$qtLibraryTarget($$TARGET)
TARGETPATH = $$[QT_INSTALL_LIBS]

include(../defaults.pri)

CONFIG += qt create_pc create_prl no_install_prl link_pkgconfig
QT += gui
PKGCONFIG += qt5embedwidget sailfishsilica

SOURCES += downloadhelper.cpp \
           logging.cpp \
           webengine.cpp \
           webenginesettings.cpp

HEADERS += downloadhelper.h \
           logging.h \
           webengine.h \
           webengine_p.h \
           webenginesettings.h \
           webenginesettings_p.h

develheaders.path = /usr/include/libsailfishwebengine
develheaders.files = downloadhelper.h \
                     webengine.h \
                     webenginesettings.h

target.path = $$[QT_INSTALL_LIBS]
pkgconfig.files = $$TARGET.pc
pkgconfig.path = $$target.path/pkgconfig

QMAKE_PKGCONFIG_NAME = lib$$TARGET
QMAKE_PKGCONFIG_DESCRIPTION = Sailfish OS WebEngine development files
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$develheaders.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_REQUIRES = Qt5Core qt5embedwidget

INSTALLS += target develheaders pkgconfig
