TEMPLATE=subdirs
SUBDIRS+=webengine webview pickers popups controls

webview.depends = webengine
pickers.depends = webview
popups.depends = webview
