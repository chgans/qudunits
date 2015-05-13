QUU_VERSION = 0.0.1
QUU_VERSION_MAJ = $$section(QBS_VERSION, ., 0, 0)
DEFINES += QUU_VERSION=\\\"$$UU_VERSION\\\"

TEMPLATE = subdirs
SUBDIRS = \
    src \
    tests

include(doc/doc.pri)
