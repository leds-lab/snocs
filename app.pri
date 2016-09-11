TEMPLATE = app

include(common.pri)

CONFIG += console

unix {
    LIBS += -ldl
}
