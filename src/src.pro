#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T17:02:22
#
#-------------------------------------------------

QT       -= gui
CONFIG +=  c++11
TARGET = qudunit
TEMPLATE = lib

LIBS += -ludunits2

DEFINES += QUDUNIT_LIBRARY

SOURCES += qudunit.cpp

HEADERS += qudunit.h\
        qudunit_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
