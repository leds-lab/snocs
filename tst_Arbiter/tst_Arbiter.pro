include(../app.pri)

TARGET = tst_arbiter

SOURCES += \
    tst_arbiter.cpp

OBJECTS += ../objs/ProgrammablePriorityEncoder.o \
    ../objs/Arbiter.o

