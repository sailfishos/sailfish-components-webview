TEMPLATE=subdirs
SUBDIRS+=lib import import/popups import/pickers
OTHER_FILES += $$PWD/rpm/sailfish-components-webview.spec

import.depends = lib
import/popups.depends = import
import/pickers.depends = import
