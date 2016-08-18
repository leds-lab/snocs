CONFIG -= app_bundle
CONFIG -= qt
TEMPLATE = app
CONFIG += console
TARGET = tst_arbiter

include(../common.pri)

SOURCES += \
    tst_arbiter.cpp

OBJECTS += ../objs/ProgrammablePriorityEncoder.o \
    ../objs/Arbiter.o
