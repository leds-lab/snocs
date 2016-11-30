/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : Routing_XYZ
FILE   : Routing_XYZ.h
--------------------------------------------------------------------------------
DESCRIPTION: Implements the XYZ routing algorithm for 3D topology.
It takes as parameters the router's coordinates and compares them with the
destination router's coordi-specified on the header's RIB field.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 29/11/2016 - 1.0     - Eduardo Alves da Silva      | First implementation
--------------------------------------------------------------------------------
*/
#ifndef __ROUTING_XYZ_H__
#define __ROUTING_XYZ_H__

#include "../Routing/Routing.h"

/*!
 * \brief The Routing_XYZ class implements the XYZ routing algorithm
 * for Mesh 3D topology.
 *
 * Implements the XYZ routing algorithm. It takes as parameters the
 * router's coordinates and compares them with the destination
 * router's coordinate-specified on the header's RIB field.
 */
class Routing_XYZ : public IOrthogonal3DRouting {
public:
    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_XYZ);
    Routing_XYZ(sc_module_name mn,
                unsigned short nPorts,
                unsigned short ROUTER_ID);

    const char* moduleName() const { return "Routing_XYZ"; }
};

#endif // __ROUTING_XYZ_H__
