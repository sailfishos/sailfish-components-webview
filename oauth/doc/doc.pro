TEMPLATE = aux

CONFIG += mer-qdoc-template
MER_QDOC.project = sailfish-oauth
MER_QDOC.config = sailfish-oauth.qdocconf
MER_QDOC.style = offline
MER_QDOC.path = /usr/share/doc/sailfish-oauth/

OTHER_FILES += \
    $$PWD/sailfish-oauth.qdocconf \
    $$PWD/sailfish-oauth.cpp \
    $$PWD/sailfish-oauth-oauth1.qdoc \
    $$PWD/sailfish-oauth-oauth10a.qdoc \
    $$PWD/sailfish-oauth-oauth2.qdoc \
    $$PWD/sailfish-oauth-oauth2acpkce.qdoc \
    $$PWD/sailfish-oauth-oauth2ac.qdoc \
    $$PWD/sailfish-oauth-oauth2implicit.qdoc \
    $$PWD/index.qdoc
