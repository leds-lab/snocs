/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : Routing_NF
FILE   : Routing_NF.h
--------------------------------------------------------------------------------
DESCRIPTION: Implements the NF routing algorithm. It takes as parameters the
router's coordinates and compares them with the destination router's coordi-
specified on the header's RIB field.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 01/12/2016 - 1.0     - Sérgio Vargas Júnior        | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef __ROUTING_NF_H__
#define __ROUTING_NF_H__

#include "../src/Routing.h"

/*!
 * \brief The Routing_NF class implements the Negative-First routing algorithm
 * for Mesh 2D topology.
 *
 * Implements the Negative-First routing algorithm. It takes as parameters the
 * router's coordinates and compares them with the destination
 * router's coordinate-specified on the header's RIB field.
 */
class Routing_NF : public IOrthogonal2DRouting {
public:
    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_NF);
    Routing_NF(sc_module_name mn,
               unsigned short nPorts,
               unsigned short ROUTER_ID,
               unsigned short PORT_ID);

    const char* moduleName() const { return "Routing_NF"; }
};

#endif // __ROUTING_NF_H__
