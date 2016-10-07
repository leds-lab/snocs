/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : PG_RoundRobin
FILE   : PG_RoundRobin.h
--------------------------------------------------------------------------------
DESCRIPTION: That is a function which determines the next priority levels by
implementing a round-robin algorithm. At each clock cycle, defined by a new
grant to a pending request, it rotates left the current grants status and
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
#ifndef __PG_ROUNDROBIN_H__
#define __PG_ROUNDROBIN_H__

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

    ModuleType moduleType() const { return SoCINModule::TPriorityGenerator; }
    const char* moduleName() const { return "PG_RoundRobin"; }
};


#endif // __PG_ROUNDROBIN_H__
