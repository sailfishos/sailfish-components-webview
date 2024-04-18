TEMPLATE = subdirs
SUBDIRS += auto
OTHER_FILES += $$PWD/test-definition/tests.xml

xml.path = /opt/tests/sailfish-components-webview/test-definition/
xml.files = $$PWD/test-definition/tests.xml
INSTALLS += xml
