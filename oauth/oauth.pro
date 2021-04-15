TEMPLATE=subdirs
SUBDIRS+=lib import examples doc
import.depends = lib
examples.depends = lib
doc.depends = lib
