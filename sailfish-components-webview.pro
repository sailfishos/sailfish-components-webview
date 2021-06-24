TEMPLATE=subdirs
SUBDIRS+=lib import tests examples doc tools
OTHER_FILES += $$PWD/rpm/sailfish-components-webview.spec

import.depends = lib
tests.depends = import
doc.depends = lib import
