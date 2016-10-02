TEMPLATE = app

include(common.pri)
include(resources.pri)

unix {
    LIBS += -ldl
}
