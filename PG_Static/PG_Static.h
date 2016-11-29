/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : PG_Static
FILE   : PG_Static.h
--------------------------------------------------------------------------------
DESCRIPTION: That is a function which determines the next priority levels by
implementing a static algorithm. Always the Request(0) has highest priority
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 12/07/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef __PG_STATIC_H__
#define __PG_STATIC_H__

#include "../PriorityGenerator/PriorityGenerator.h"

class PG_Static : public IPriorityGenerator {
public:

    SC_HAS_PROCESS(PG_Static);
    PG_Static(sc_module_name mn, unsigned int priority,
              unsigned short int numReqs_Grants,
              unsigned short int ROUTER_ID,
              unsigned short int PORT_ID);

    ModuleType moduleType() const { return SoCINModule::TPriorityGenerator; }
    const char* moduleName() const { return "PG_Static"; }

};


#endif // PG_STATIC_H
