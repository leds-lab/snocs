TEMPLATE = subdirs

# What subproject depends on others
tst_Arbiter.depends = Arbiter ProgrammablePriorityEncoder

SUBDIRS += \
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
    Memory
