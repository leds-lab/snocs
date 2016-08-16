TEMPLATE = subdirs

SUBDIRS += \
    PriorityGenerator \
    PG_Static \
    PG_Rotative \
    PG_RoundRobin \
    PG_Tester \
    ProgrammablePriorityEncoder \
    Arbiter \
    Arbiter_Tester

CONFIG += ordered

# What subproject depends on others
Arbiter_Tester.depends = Arbiter ProgrammablePriorityEncoder
