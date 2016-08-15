#ifndef PG_STATIC_H
#define PG_STATIC_H

#include "../PriorityGenerator/PriorityGenerator.h"

class PG_Static : public IPriorityGenerator {
public:

    SC_HAS_PROCESS(PG_Static);
    PG_Static(sc_module_name mn, unsigned int priority,
              unsigned short int numReqs_Grants,
              unsigned short int XID,
              unsigned short int YID,
              unsigned short int PORT_ID);

    const char* moduleTypeName() { return "PG_Static"; }

};

extern "C" {
    SS_EXP IPriorityGenerator* new_PG(sc_simcontext* simcontext,
                              sc_module_name moduleName,
                              unsigned int numReqs_Grants,
                              unsigned short int XID,
                              unsigned short int YID,
                              unsigned short int PORT_ID) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new PG_Static(moduleName,0,numReqs_Grants,XID,YID,PORT_ID);
    }
    SS_EXP void delete_PG(IPriorityGenerator* pg) {
        delete pg;
    }
}

#endif // PG_STATIC_H
