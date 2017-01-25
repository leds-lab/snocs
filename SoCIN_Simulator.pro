TEMPLATE = subdirs

SUBDIRS += \
    Parameters \
    PriorityGenerator \
    PG_Static \
    PG_Rotative \
    PG_RoundRobin \
    tst_PG \
    ProgrammablePriorityEncoder \
    Arbiter \
    tst_Arbiter \
    tst_ProgrammablePriorityEncoder \
    FlowControl \
    FC_Handshake \
    FC_CreditBased \
    Memory \
    FIFO \
    Multiplexers \
    tst_Multiplexrers \
    Demultiplexers \
    tst_Demultiplexers \
    Routing \
    Simulator \
    Routing_XY \
    RequestRegister \
    InputController \
    tst_InputController \
    OutputController \
    XIN \
    AndParam \
    tst_AndParam \
    XOUT \
    Router \
    ParIS \
    PluginManager \
    tst_PluginManager \
    NoC \
    SoCIN \
    StopSim \
    SystemSignals \
    VcPriorityEncoder \
    TrafficMeter \
    SoCIN_single_unit \
    Routing_Crossbar \
    ParIS_Bus \
    SoCIN_Torus \
    Routing_DOR_Torus \
    SoCIN_Ring \
    Routing_Ring \
    SoCIN_Spider \
    Routing_Crossfirst \
    SoCIN_3D \
    Routing_XYZ \
    Routing_Ring_zero \
    Routing_NF \
    Routing_WF \
    Routing_NL

OTHER_FILES += \
    app.pri \
    common.pri \
    plugin.pri \
    resources.pri \
    socindefines.pri \
    export.h \
    SoCINModule.h \
    SoCINDefines.h \
    simconf.conf
