TEMPLATE = aux

CONFIG += mer-qdoc-template
MER_QDOC.project = sailfish-webview
MER_QDOC.config = sailfish-webview.qdocconf
MER_QDOC.style = offline
MER_QDOC.path = /usr/share/doc/sailfish-components-webview/

OTHER_FILES += \
    $$PWD/sailfish-webview.qdocconf \
    $$PWD/sailfish-webview.cpp \
    $$PWD/sailfish-webview-webview.qdoc \
    $$PWD/sailfish-webview-webviewadditional.qdoc \
    $$PWD/sailfish-webview-webviewexperimental.qdoc \
    $$PWD/sailfish-webview-webviewflickable.qdoc \
    $$PWD/sailfish-webview-webviewpage.qdoc \
    $$PWD/sailfish-webview-webengine.qdoc \
    $$PWD/sailfish-webview-webengine.cpp \
    $$PWD/sailfish-webview-webengine.h \
    $$PWD/sailfish-webview-webenginesettings.qdoc \
    $$PWD/sailfish-webview-webenginesettings.cpp \
    $$PWD/sailfish-webview-webenginesettings.h \
    $$PWD/sailfish-webview-popups-popupprovider.qdoc \
    $$PWD/sailfish-webview-popups-alertpopupinterface.qdoc \
    $$PWD/sailfish-webview-popups-authpopupinterface.qdoc \
    $$PWD/sailfish-webview-popups-confirmpopupinterface.qdoc \
    $$PWD/sailfish-webview-popups-contextmenuinterface.qdoc \
    $$PWD/sailfish-webview-popups-locationpopupinterface.qdoc \
    $$PWD/sailfish-webview-popups-passwordmanagerpopupinterface.qdoc \
    $$PWD/sailfish-webview-popups-promptpopupinterface.qdoc \
    $$PWD/sailfish-webview-popups-userpromptinterface.qdoc \
    $$PWD/sailfish-webview-popups-blockedpopupinterface.qdoc \
    $$PWD/index.qdoc
