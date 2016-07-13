#include "PG_Static.h"

PG_Static::PG_Static(sc_module_name mn,unsigned int priority,
                     unsigned int numReqs_Grants,
                     unsigned short int XID,
                     unsigned short int YID,
                     unsigned short int PORT_ID)
        : PriorityGenerator(mn,numReqs_Grants,XID,YID,PORT_ID)
{

    if(priority >= numReqs_Grants) {
        o_PRIORITIES[0].initialize(true);
        for(unsigned int i = 1; i < numReqs_Grants; i++) {
            o_PRIORITIES[i].initialize(false);
        }
    } else {
        for(unsigned int i = 0; i < numReqs_Grants; i++) {
            if(i != priority) {
                o_PRIORITIES[i].initialize(false);
            } else {
                o_PRIORITIES[i].initialize(true);
            }
        }
    }

}
