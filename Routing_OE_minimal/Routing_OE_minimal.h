/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : Routing_OE_minimal
FILE   : Routing_OE_minimal.h
--------------------------------------------------------------------------------
DESCRIPTION: Implements the OE routing algorithm. It takes as parameters the
router's coordinates and compares them with the destination router's coordi-
specified on the header's RIB field.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 01/02/2017 - 1.0     - Sérgio Vargas Júnior        | First implementation
--------------------------------------------------------------------------------
*/
#ifndef __ROUTING_OE_MINIMAL_H__
#define __ROUTING_OE_MINIMAL_H__

#include "../src/Routing.h"

/*!
 * \brief The Routing_OE_minimal class implements the Odd-Even routing algorithm
 * for Mesh 2D topology.
 *
 * Implements the Odd-Even minimal routing algorithm. It takes as parameters the
 * router's coordinates and compares them with the destination
 * router's coordinate-specified on the header's RIB field.
 */
class Routing_OE_minimal : public IOrthogonal2DRouting {
public:
    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_OE_minimal);
    Routing_OE_minimal(sc_module_name mn,
               unsigned short nPorts,
               unsigned short ROUTER_ID,
               unsigned short PORT_ID);

    const char* moduleName() const { return "Routing_OE"; }
};

#endif // __ROUTING_OE_H__
