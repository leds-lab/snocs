#ifndef PG_ROUNDROBIN_H
#define PG_ROUNDROBIN_H

#include "../PriorityGenerator/PriorityGenerator.h"

/////////////////////////////////////////

class PG_RoundRobin : public IPriorityGenerator {
public:

    // Internal signals
    sc_signal<bool>             r_UPDATE;          // Command to up to date Preg
    sc_vector<sc_signal<bool> > r_GDELAYED;        // G delayed in 1 cycle
    sc_vector<sc_signal<bool> > r_NEXT_PRIORITIES; // Next priorities values
    sc_vector<sc_signal<bool> > r_PRIORITIES;      // Priorities register

    // Module's processes
    void p_GDELAYED();
    void p_UPDATE();
    void p_NEXT_PRIORITIES();
    void p_PRIORITIES();
    void p_OUTPUTS();

    SC_HAS_PROCESS(PG_RoundRobin);
    PG_RoundRobin(sc_module_name mn,
              unsigned int numReqs_Grants,
              unsigned short int XID,
              unsigned short int YID,
              unsigned short int PORT_ID);

    const char* moduleTypeName() { return "PG_RoundRobin"; }
};

extern "C" {
    SS_EXP IPriorityGenerator* new_PG(sc_simcontext* simcontext,
            sc_module_name moduleName,unsigned short int numReqs_Grants,
            unsigned short int XID,unsigned short int YID, 
			unsigned short int PORT_ID) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new PG_RoundRobin(moduleName,numReqs_Grants,XID,YID,PORT_ID);
    }
    SS_EXP void delete_PG(IPriorityGenerator* pg) {
        delete pg;
    }
}


#endif // PG_ROUNDROBIN_H
