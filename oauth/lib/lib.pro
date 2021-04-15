TEMPLATE = lib
TARGET = sailfishoauth
TARGET = $$qtLibraryTarget($$TARGET)
TARGETPATH = $$[QT_INSTALL_LIBS]

include(../../defaults.pri)

CONFIG += qt create_pc create_prl no_install_prl link_pkgconfig
QT += qml network

DEFINES += SAILFISHOS_OAUTH_LIBRARY_BUILD

SOURCES += oauth1.cpp \
           oauth2.cpp \
           oautherror.cpp \
           redirectlistener.cpp \
           redirecturiparser.cpp

PUBLIC_HEADERS = oauthsymbols.h \
                 oauth1.h \
                 oauth2.h \
                 oautherror.h \
                 redirectlistener.h
PRIVATE_HEADERS = redirecturiparser_p.h
HEADERS += $$PUBLIC_HEADERS \
           $$PRIVATE_HEADERS

develheaders.path = /usr/include/libsailfishoauth
develheaders.files = $$PUBLIC_HEADERS

target.path = $$[QT_INSTALL_LIBS]
pkgconfig.files = $$TARGET.pc
pkgconfig.path = $$target.path/pkgconfig

QMAKE_PKGCONFIG_NAME = lib$$TARGET
QMAKE_PKGCONFIG_DESCRIPTION = Sailfish OS OAuth helpers development files
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$develheaders.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_REQUIRES = Qt5Core Qt5Qml Qt5Network

INSTALLS += target develheaders pkgconfig
