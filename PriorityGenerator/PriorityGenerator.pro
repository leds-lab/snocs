#-------------------------------------------------
#
# Project created by QtCreator 2016-07-12T20:22:52
#
#-------------------------------------------------

CONFIG -= qt
CONFIG -= app_bundle

TARGET = PriorityGenerator
TEMPLATE = lib

include(../common.pri)

DEFINES += PRIORITYGENERATOR_LIBRARY

SOURCES += PriorityGenerator.cpp \
    PG_Static.cpp

HEADERS += PriorityGenerator.h\
        prioritygenerator_global.h \
    PG_Static.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
