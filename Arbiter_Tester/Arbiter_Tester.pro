CONFIG -= app_bundle
CONFIG -= qt
TEMPLATE = app
CONFIG += console
TARGET = Arb_Tester

include(../common.pri)

SOURCES += \
    tst_arbiter.cpp

OBJECTS += ../objs/ProgrammablePriorityEncoder.o \
    ../objs/Arbiter.o
