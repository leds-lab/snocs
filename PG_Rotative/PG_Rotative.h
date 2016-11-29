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
#ifndef __PG_ROTATIVE_H__
#define __PG_ROTATIVE_H__

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

    ModuleType moduleType() const { return  SoCINModule::TPriorityGenerator; }
    const char* moduleName() const { return "PG_Rotative"; }

    SC_HAS_PROCESS(PG_Rotative);
    /*!
     * \brief PG_Rotative
     * \param mn
     * \param numReqs_Grants
     * \param ROUTER_ID
     * \param PORT_ID
     */
    PG_Rotative(sc_module_name mn,
              unsigned short int numReqs_Grants,
              unsigned short int ROUTER_ID,
              unsigned short int PORT_ID);

};


#endif // __PG_ROTATIVE_H__
