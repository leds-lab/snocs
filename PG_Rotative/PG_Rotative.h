#ifndef PG_ROTATIVE_H
#define PG_ROTATIVE_H

#include "../PriorityGenerator/PriorityGenerator.h"

class PG_SHARED_EXPORT PG_Rotative : public PriorityGenerator {
public:

    // Internal signals
    sc_signal<bool>             update_register; // Command to up to date Preg
    sc_vector<sc_signal<bool> > Gdelayed;        // G delayed in 1 cycle
    sc_vector<sc_signal<bool> > nextP;           // Next priorities values
    sc_vector<sc_signal<bool> > Preg;            // Priorities register

    // Module's processes
    void p_gdelayed();
    void p_update_register();
    void p_nextp();
    void p_preg();
    void p_outputs();
    void p_debug();

    SC_HAS_PROCESS(PG_Rotative);
    PG_Rotative(sc_module_name mn,
              unsigned int numReqs_Grants,
              unsigned short int XID,
              unsigned short int YID,
              unsigned short int PORT_ID);

};

extern "C" {
    PriorityGenerator* new_PG_Rotative(sc_simcontext* simcontext,
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

        return new PG_Rotative(moduleName,numReqs_Grants,XID,YID,PORT_ID);
    }
    void delete_PG_Rotative(PriorityGenerator* pg) {
        delete pg;
    }
}


#endif // PG_STATIC_H
