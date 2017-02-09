#include "PG_Random.h"
#include "../export.h"

#include "../SoCINDefines.h"
#include "../Parameters/Parameters.h"

PG_Random::PG_Random(sc_module_name mn,
                     unsigned short numReqs_Grants,
                     unsigned short int ROUTER_ID,
                     unsigned short int PORT_ID)
        : IPriorityGenerator(mn,numReqs_Grants,ROUTER_ID,PORT_ID),
        randomGenerator(SEED)
{
    uniformRandom = std::uniform_int_distribution<int>(0, numReqs_Grants-1);
    srand(SEED);

    SC_METHOD(p_OUTPUTS);
    sensitive << i_CLK.pos() << i_RST;

}

////////////////////////////////////////////////////////////////////////////////
void PG_Random::p_OUTPUTS()
////////////////////////////////////////////////////////////////////////////////
// It updates the output
{
    UIntVar v_P_tmp(0,numPorts);
//    v_P_tmp = pow(2, rand() % numPorts);
    v_P_tmp = pow(2, uniformRandom(randomGenerator));
    for(unsigned short int i = 0; i < numPorts; i++) {
        o_PRIORITIES[i].write( v_P_tmp[i] );
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
    SS_EXP IPriorityGenerator* new_PG(sc_simcontext* simcontext,
                              sc_module_name moduleName,
                              unsigned short int numReqs_Grants,
                              unsigned short int ROUTER_ID,
                              unsigned short int PORT_ID) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new PG_Random(moduleName,numReqs_Grants,ROUTER_ID,PORT_ID);
    }
    SS_EXP void delete_PG(IPriorityGenerator* pg) {
        delete pg;
    }
}

