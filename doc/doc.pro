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
    $$PWD/sailfish-webview-webviewflickable.qdoc \
    $$PWD/sailfish-webview-webviewpage.qdoc \
    $$PWD/index.qdoc
