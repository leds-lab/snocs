TEMPLATE = app

CONFIG -= app_bundle
CONFIG -= qt
CONFIG += console

include(common.pri)

unix {
    LIBS += -ldl
}
