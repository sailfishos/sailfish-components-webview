
# define library include path based on target arch
DEFINES += SAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH=\"\\\"$$[QT_INSTALL_LIBS]/mozembedlite\\\"\"

QMAKE_CXXFLAGS += -Wparentheses -Werror -Wfatal-errors
CONFIG += qt c++11
QT += core
