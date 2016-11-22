TARGET = SNoCS
CONFIG += c++11

include(../app.pri)
include(../socindefines.pri)

SOURCES += \
    main.cpp \
    ../PluginManager/PluginManager.cpp \
    ../SystemSignals/SystemSignals.cpp \
    ../StopSim/StopSim.cpp \
    ../TrafficMeter/TrafficMeter.cpp \
    fg.cpp \
    UnboundedFifo.cpp
#    tm_single.cpp

HEADERS += \
    ../PluginManager/PluginManager.h \
    ../SystemSignals/SystemSignals.h \
    ../StopSim/StopSim.h \
    ../TrafficMeter/TrafficMeter.h \
    fg.h \
    tg.h \
    UnboundedFifo.h
#    tm_single.h

OTHER_FILES += simconf.conf
