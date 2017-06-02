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

#include "../src/PriorityGenerator.h"
/*!
 * \brief The PG_Rotative class implements a rotative
 * priority generator that rotates the priorities in
 * each arbitration
 */
class PG_Rotative : public IPriorityGenerator {
public:

    // Internal signals
    sc_signal<bool>             r_UPDATE;           //! Command to up to date Preg
    sc_vector<sc_signal<bool> > r_GDELAYED;         //! G delayed in 1 cycle
    sc_vector<sc_signal<bool> > r_NEXT_PRIORITIES;  //! Next priorities values
    sc_vector<sc_signal<bool> > r_PRIORITIES;       //! Priorities register

    // Module's processes
    void p_GDELAYED();
    void p_UPDATE();
    void p_NEXT_PRIORITIES();
    void p_PRIORITIES();
    void p_OUTPUTS();

    SC_HAS_PROCESS(PG_Rotative);
    PG_Rotative(sc_module_name mn,
              unsigned short int numReqs_Grants,
              unsigned short int ROUTER_ID,
              unsigned short int PORT_ID);

    ModuleType moduleType() const { return  SoCINModule::TPriorityGenerator; }
    const char* moduleName() const { return "PG_Rotative"; }
};


#endif // __PG_ROTATIVE_H__
