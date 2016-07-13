#ifndef PG_STATIC_H
#define PG_STATIC_H

#include "../PriorityGenerator/PriorityGenerator.h"

class PG_SHARED_EXPORT PG_Static : public PriorityGenerator {
public:

    SC_HAS_PROCESS(PG_Static);
    PG_Static(sc_module_name mn, unsigned int priority,
              unsigned int numReqs_Grants,
              unsigned short int XID,
              unsigned short int YID,
              unsigned short int PORT_ID);

    const char* moduleName() { return "PG_Static"; }

};

extern "C" {
    PriorityGenerator* new_PG_Static(sc_simcontext* simcontext,
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

        return new PG_Static(moduleName,1,numReqs_Grants,XID,YID,PORT_ID);
    }
    void delete_PG_Static(PriorityGenerator* pg) {
        delete pg;
    }
}


#endif // PG_STATIC_H
