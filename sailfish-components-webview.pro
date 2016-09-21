TEMPLATE=subdirs
SUBDIRS+=lib import
OTHER_FILES += $$PWD/rpm/sailfish-components-webview.spec

import.depends = lib
