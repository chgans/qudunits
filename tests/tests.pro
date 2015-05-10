#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T17:03:12
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_udunits2
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    tst_udunits2.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../src/release/ -lqudunit
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../src/debug/ -lqudunit
else:unix: LIBS += -L$$OUT_PWD/../src/ -lqudunit -ludunits2

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src
