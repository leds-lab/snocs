TARGET = Simulator

include(../app.pri)
include(../socindefines.pri)

SOURCES += \
    main.cpp \
    ../PluginManager/PluginManager.cpp \
    ../SystemSignals/SystemSignals.cpp \
    ../StopSim/StopSim.cpp \
    fg.cpp \
    fifo_in.cpp \
    fifo_out.cpp \
#    global_clock.cpp \
    ifc_credit.cpp \
    ifc_credit_vct.cpp \
    ifc_handshake.cpp \
    ofc_credit.cpp \
    ofc_credit_vct.cpp \
    ofc_handshake.cpp \
#    stopsim.cpp \
    tm_single.cpp

HEADERS += \
    ../PluginManager/PluginManager.h \
    ../SystemSignals/SystemSignals.h \
    ../StopSim/StopSim.h \
    fg.h \
    fifo_in.h \
    fifo_out.h \
#    global_clock.h \
    ifc.h \
    ifc_credit.h \
    ifc_credit_vct.h \
    ifc_handshake.h \
    ofc.h \
    ofc_credit.h \
    ofc_credit_vct.h \
    ofc_handshake.h \
#    reset.h \
#    stopsim.h \
    tg.h \
    tm_single.h
