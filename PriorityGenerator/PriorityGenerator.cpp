#include "PriorityGenerator.h"

PriorityGenerator::PriorityGenerator(sc_module_name mn,
                                     unsigned int numReqs_Grants,
                                     unsigned short int XID,
                                     unsigned short int YID,
                                     unsigned short int PORT_ID)
                : sc_module(mn), i_CLK("PG_CLK"), i_RST("PG_RESET"),
                  i_GRANTS("PG_GRANTS",numReqs_Grants),
                  o_PRIORITIES("PG_PRIORITIES",numReqs_Grants),
                  numPorts(numReqs_Grants)
{}
