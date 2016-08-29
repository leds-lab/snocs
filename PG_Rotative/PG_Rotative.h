/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : PG_Rotative
FILE   : PG_Rotative.h
--------------------------------------------------------------------------------
DESCRIPTION: That is a function which determines the next priority levels by
implementing a rotative algorithm. At each clock cycle, defined by a new
grant to a pending request, it rotates left the current priority status and
ensures that the request being granted will have the lowest priority level at
the next arbitration cycle.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 12/07/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef PG_ROTATIVE_H
#define PG_ROTATIVE_H

#include "../PriorityGenerator/PriorityGenerator.h"
/*!
 * \brief The PG_Rotative class implements a rotative
 * priority generator that rotates the priorities in
 * each arbitration
 */
class PG_Rotative : public IPriorityGenerator {
public:

    // Internal signals
    sc_signal<bool>             update_register; //! Command to up to date Preg
    sc_vector<sc_signal<bool> > Gdelayed;        //! G delayed in 1 cycle
    sc_vector<sc_signal<bool> > nextP;           //! Next priorities values
    sc_vector<sc_signal<bool> > Preg;            //! Priorities register

    // Module's processes
    void p_gdelayed();
    void p_update_register();
    void p_nextp();
    void p_preg();
    void p_outputs();
    void p_debug();

    ModuleType moduleType() const { return  SoCINModule::PriorityGenerator; }
    const char* moduleName() const { return "PG_Rotative"; }

    SC_HAS_PROCESS(PG_Rotative);
    /*!
     * \brief PG_Rotative
     * \param mn
     * \param numReqs_Grants
     * \param XID
     * \param YID
     * \param PORT_ID
     */
    PG_Rotative(sc_module_name mn,
              unsigned short int numReqs_Grants,
              unsigned short int XID,
              unsigned short int YID,
              unsigned short int PORT_ID);

};

extern "C" {
    SS_EXP IPriorityGenerator* new_PG(sc_simcontext* simcontext,
                              sc_module_name moduleName,
                              unsigned short int numReqs_Grants,
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
    SS_EXP void delete_PG(IPriorityGenerator* pg) {
        delete pg;
    }
}


#endif // PG_ROTATIVE_H
