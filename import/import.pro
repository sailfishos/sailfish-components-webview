TEMPLATE=subdirs
SUBDIRS+=webengine webview pickers popups

webview.depends = webengine
pickers.depends = webview
popups.depends = webview
