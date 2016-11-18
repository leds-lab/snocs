TARGET = SNoCS

include(../app.pri)
include(../socindefines.pri)

SOURCES += \
    main.cpp \
    ../PluginManager/PluginManager.cpp \
    ../SystemSignals/SystemSignals.cpp \
    ../StopSim/StopSim.cpp \
    ../TrafficMeter/TrafficMeter.cpp \
    fg.cpp \
    unboundedfifo.cpp \
    ifc_credit.cpp \
    ifc_credit_vct.cpp \
    ifc_handshake.cpp \
    ofc_credit.cpp \
    ofc_credit_vct.cpp \
    ofc_handshake.cpp
#    tm_single.cpp

HEADERS += \
    ../PluginManager/PluginManager.h \
    ../SystemSignals/SystemSignals.h \
    ../StopSim/StopSim.h \
    ../TrafficMeter/TrafficMeter.h \
    fg.h \
    unboundedfifo.h \
    ifc.h \
    ifc_credit.h \
    ifc_credit_vct.h \
    ifc_handshake.h \
    ofc.h \
    ofc_credit.h \
    ofc_credit_vct.h \
    ofc_handshake.h \
    tg.h
#    tm_single.h

OTHER_FILES += simconf.conf
