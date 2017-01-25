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
    UnboundedFifo.cpp \
    FlowGenerator.cpp \
    DestinationGenerator.cpp \
    UniformDistribution.cpp \
    TypeInjection.cpp \
    VarPacketSizeFixIdle.cpp \
    VarIntervalFixPacketSize.cpp \
    VarPacketSizeFixInterval.cpp \
    VarIdleFixPacketSize.cpp \
    VarBurstFixInterval.cpp \
    ConstantInjection.cpp
#    tm_single.cpp

HEADERS += \
    ../PluginManager/PluginManager.h \
    ../SystemSignals/SystemSignals.h \
    ../StopSim/StopSim.h \
    ../TrafficMeter/TrafficMeter.h \
    UnboundedFifo.h \
    TerminalInstrumentation.h \
    FlowGenerator.h \
    DestinationGenerator.h \
    UniformDistribution.h \
    TypeInjection.h \
    VarPacketSizeFixIdle.h \
    VarIntervalFixPacketSize.h \
    VarPacketSizeFixInterval.h \
    VarIdleFixPacketSize.h \
    VarBurstFixInterval.h \
    ConstantInjection.h
#    tm_single.h

OTHER_FILES += simconf.conf
