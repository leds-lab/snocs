TEMPLATE = app

include(common.pri)

unix {
    LIBS += -ldl
}
