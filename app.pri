CONFIG -= app_bundle
CONFIG -= qt
TEMPLATE = app
CONFIG += console

include(common.pri)

unix {
    LIBS += -ldl
}
