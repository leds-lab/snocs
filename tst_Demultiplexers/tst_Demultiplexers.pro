TEMPLATE = app
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += console
TARGET = tst_demultiplexers

include(../common.pri)


OBJECTS += ../objs/demultiplexers.o

SOURCES += \
    tst_demultiplexers.cpp

HEADERS +=
