CONFIG -= app_bundle
CONFIG -= qt
TEMPLATE = app
CONFIG += console
TARGET = tst_PPE

include(../common.pri)

SOURCES += \
    tst_ppe.cpp

OBJECTS += ../objs/ProgrammablePriorityEncoder.o
