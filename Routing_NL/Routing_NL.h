/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : Routing_NL
FILE   : Routing_NL.h
--------------------------------------------------------------------------------
DESCRIPTION: Implements the NL routing algorithm. It takes as parameters the
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
#ifndef __ROUTING_NL_H__
#define __ROUTING_NL_H__

#include "../Routing/Routing.h"

/*!
 * \brief The Routing_NL class implements the North-Last routing algorithm
 * for Mesh 2D topology.
 *
 * Implements the North-Last routing algorithm. It takes as parameters the
 * router's coordinates and compares them with the destination
 * router's coordinate-specified on the header's RIB field.
 */
class Routing_NL : public IOrthogonal2DRouting {
public:
    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_NL);
    Routing_NL(sc_module_name mn,
               unsigned short nPorts,
               unsigned short ROUTER_ID);

    const char* moduleName() const { return "Routing_NL"; }
};

#endif // __ROUTING_NL_H__
