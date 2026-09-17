
# define library include path based on target arch
DEFINES += SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH=\"\\\"$$[QT_INSTALL_LIBS]/gecko-embedlite\\\"\"

QMAKE_CXXFLAGS += -Wparentheses -Werror -Wfatal-errors
CONFIG += qt
QT += core
