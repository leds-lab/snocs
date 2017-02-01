/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : Routing_OE
FILE   : Routing_OE.h
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
#ifndef __ROUTING_OE_H__
#define __ROUTING_OE_H__

#include "../Routing/Routing.h"

/*!
 * \brief The Routing_OE class implements the Odd-Even routing algorithm
 * for Mesh 2D topology.
 *
 * Implements the Odd-Even routing algorithm. It takes as parameters the
 * router's coordinates and compares them with the destination
 * router's coordinate-specified on the header's RIB field.
 */
class Routing_OE : public IOrthogonal2DRouting {
public:
    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_OE);
    Routing_OE(sc_module_name mn,
               unsigned short nPorts,
               unsigned short ROUTER_ID,
               unsigned short PORT_ID);

    const char* moduleName() const { return "Routing_OE"; }
};

#endif // __ROUTING_OE_H__
