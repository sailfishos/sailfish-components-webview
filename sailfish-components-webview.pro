TEMPLATE=subdirs
SUBDIRS+=lib import tests
OTHER_FILES += $$PWD/rpm/sailfish-components-webview.spec

import.depends = lib
tests.depends = import
