TEMPLATE = app
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += console
TARGET = tst_multiplexers

include(../common.pri)


OBJECTS += ../objs/Multiplexers.o


SOURCES += \
    tst_multiplexers.cpp

HEADERS +=
