CONFIG -= app_bundle
CONFIG -= qt
TEMPLATE = app
CONFIG += console

SOURCES += \
    tst_pg.cpp

include(../common.pri)

unix {
    LIBS += -ldl
}
