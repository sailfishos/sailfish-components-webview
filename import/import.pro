TEMPLATE=subdirs
SUBDIRS+=webengine webview pickers popups controls

webview.depends = webengine
pickers.depends = webengine
popups.depends = webengine

controls.depends = webengine
