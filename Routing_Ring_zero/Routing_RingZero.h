#ifndef ROUTING_RINGZERO_H
#define ROUTING_RINGZERO_H

#include "../src/Routing.h"

class Routing_RingZero : public IRouting {
protected:
    unsigned char REQ_NONE;
    unsigned char REQ_LOCAL;
    unsigned char REQ_CLOCKWISE;        // left
    unsigned char REQ_COUNTERCLOCKWISE; // right
public:

    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_RingZero);
    Routing_RingZero(sc_module_name mn,
                     unsigned short nPorts,
                     unsigned short ROUTER_ID,
                     unsigned short PORT_ID);
    const char* moduleName() const { return "Routing_RingZero"; }
    INoC::TopologyType supportedTopology() const { return INoC::TT_Non_Orthogonal; }
};

#endif // ROUTING_RINGZERO_H
