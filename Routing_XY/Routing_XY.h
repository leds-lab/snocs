/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : Routing_XY
FILE   : Routing_XY.h
--------------------------------------------------------------------------------
DESCRIPTION: Implements the XY routing algorithm. It takes as parameters the
router's coordinates and compares them with the destination router's coordi-
specified on the header's RIB field.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 05/09/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef ROUTING_XY_H
#define ROUTING_XY_H

#include "../Routing/Routing.h"

/*!
 * \brief The Routing_XY class implements the XY routing algorithm
 * for Mesh 2D topology.
 *
 * Implements the XY routing algorithm. It takes as parameters the
 * router's coordinates and compares them with the destination
 * router's coordinate-specified on the header's RIB field.
 */
class Routing_XY : public IMesh2DRouting {
public:
    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_XY);
    Routing_XY(sc_module_name mn,
               unsigned short nPorts,
               unsigned short XID,
               unsigned short YID);

    const char* moduleName() const { return "Routing_XY"; }
};



#endif // ROUTING_XY_H
