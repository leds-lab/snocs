TARGET = Simulator

include(../app.pri)
include(../socindefines.pri)

SOURCES += \
    main.cpp \
    ../PluginManager/PluginManager.cpp

HEADERS += \
    ../PluginManager/PluginManager.h
