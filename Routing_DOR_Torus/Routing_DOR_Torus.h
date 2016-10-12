#ifndef __ROUTING_DOR_TORUS_H__
#define __ROUTING_DOR_TORUS_H__

#include "../Routing/Routing.h"

/*!
 * \brief The Routing_DOR_Torus class implements the dimension-order
 * routing algorithm for Torus 2D topology.
 *
 * Implements the DOR routing algorithm. It takes as parameters the
 * router's coordinates and compares them with the destination
 * router's coordinate-specified on the header's RIB field.
 * This routing is deterministic, minimal-path and distributed (ParIS).
 */
class Routing_DOR_Torus : public IOrthogonal2DRouting {
public:

    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_DOR_Torus);
    Routing_DOR_Torus(sc_module_name mn,
                      unsigned short nPorts,
                      unsigned short XID,
                      unsigned short YID);

    const char* moduleName() const { return "Routing_DOR_Torus"; } // DOR: Dimension-order
};

#endif // ROUTING_DOR_TORUS_H
