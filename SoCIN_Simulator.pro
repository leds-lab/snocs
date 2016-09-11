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
    ParIS

OTHER_FILES += \
    app.pri \
    common.pri \
    plugin.pri \
    export.h \
    SoCINModule.h \
    SoCINDefines.h
