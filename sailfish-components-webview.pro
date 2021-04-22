TEMPLATE=subdirs
SUBDIRS+=lib import tests doc
OTHER_FILES += $$PWD/rpm/sailfish-components-webview.spec

import.depends = lib
tests.depends = import
doc.depends = lib import
