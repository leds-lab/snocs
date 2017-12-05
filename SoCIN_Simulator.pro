TEMPLATE = subdirs

SUBDIRS += \
    Parameters \
    FC_Handshake \
    FC_CreditBased \
    PG_Static \
    PG_Rotative \
    PG_RoundRobin \
    PG_Random \
    FIFO \
    Routing_XY \
    Routing_Crossbar \
    Routing_DOR_Torus \
    Routing_Ring \
    Routing_Crossfirst \
    Routing_XYZ \
    Routing_Ring_zero \
    Routing_NF \
    Routing_WF \
    Routing_NL \
    Routing_OE \
    Routing_OE_minimal \
    ParIS \
    ParIS_Bus \
    SoCIN \
    SoCIN_single_unit \
    SoCIN_Torus \
    SoCIN_Ring \
    SoCIN_Spider \
    SoCIN_3D \
    Simulator \
    tst_PG \
    tst_Arbiter \
    tst_ProgrammablePriorityEncoder \
    tst_Multiplexrers \
    tst_Demultiplexers \
    tst_InputController \
    tst_AndParam \
    tst_PluginManager \
    ParIS_Serial

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
