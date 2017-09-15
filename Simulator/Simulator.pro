TARGET = SNoCS
CONFIG += c++11

include(../app.pri)
include(../socindefines.pri)

SOURCES += \
    main.cpp \
    ../src/PluginManager.cpp \
    ../src/SystemSignals.cpp \
    ../src/StopSim.cpp \
    ../src/TrafficMeter.cpp \
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
    ConstantInjection.cpp \
    Simon.cpp

HEADERS += \
    ../src/PluginManager.h \
    ../src/SystemSignals.h \
    ../src/StopSim.h \
    ../src/TrafficMeter.h \
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
    ConstantInjection.h \
    Simon.h

OTHER_FILES += simconf.conf
