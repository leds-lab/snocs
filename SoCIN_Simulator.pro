TEMPLATE = subdirs

SUBDIRS += \
    PriorityGenerator \
    PG_Static \
    PG_Rotative \
    PG_RoundRobin \
    tst_PG \
    ProgrammablePriorityEncoder \
    Arbiter \
    tst_Arbiter

CONFIG += ordered

# What subproject depends on others
tst_Arbiter.depends = Arbiter ProgrammablePriorityEncoder
