#ifndef __ROUTING_CROSSFIRST_DL_FREE_H__
#define __ROUTING_CROSSFIRST_DL_FREE_H__

#include "../src/Routing.h"

class Routing_Crossfirst : public IRouting {
protected:
    unsigned char REQ_NONE;
    unsigned char REQ_LOCAL;
    unsigned char REQ_CLOCKWISE;     // left
    unsigned char REQ_ANTICLOCKWISE; // right
    unsigned char REQ_ACROSS;

    unsigned short sysSize;
    unsigned short maxHops;
public:

    sc_signal<Flit> r_DATA;

    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_Crossfirst);
    Routing_Crossfirst(sc_module_name mn,
                      unsigned short nPorts,
                      unsigned short ROUTER_ID,
                      unsigned short PORT_ID);

    const char* moduleName() const { return "Routing_Crossfirst"; }
    INoC::TopologyType supportedTopology() const { return INoC::TT_Non_Orthogonal; }

};

#endif // __ROUTING_CROSSFIRST_H__
