/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : Routing_WF
FILE   : Routing_WF.h
--------------------------------------------------------------------------------
DESCRIPTION: Implements the WF routing algorithm. It takes as parameters the
router's coordinates and compares them with the destination router's coordi-
specified on the header's RIB field.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 24/01/2017 - 1.0     - Sérgio Vargas Júnior        | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef __ROUTING_WF_H__
#define __ROUTING_WF_H__

#include "../src/Routing.h"

/*!
 * \brief The Routing_WF class implements the West-First routing algorithm
 * for Mesh 2D topology.
 *
 * Implements the West-First routing algorithm. It takes as parameters the
 * router's coordinates and compares them with the destination
 * router's coordinate-specified on the header's RIB field.
 */
class Routing_WF : public IOrthogonal2DRouting {
public:
    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_WF);
    Routing_WF(sc_module_name mn,
               unsigned short nPorts,
               unsigned short ROUTER_ID,
               unsigned short PORT_ID);

    const char* moduleName() const { return "Routing_WF"; }
};

#endif // __ROUTING_WF_H__
