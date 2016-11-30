#ifndef __ROUTING_CROSSBAR_H__
#define __ROUTING_CROSSBAR_H__

#include "../Routing/Routing.h"

class Routing_Crossbar : public IRouting {
public:

    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_Crossbar);
    Routing_Crossbar(sc_module_name mn,
                     unsigned short nPorts,
                     unsigned short ROUTER_ID);

    const char* moduleName() const { return "Routing_Crossbar"; }
    INoC::TopologyType supportedTopology() const { return INoC::TT_Non_Orthogonal; }
};

#endif // __ROUTING_CROSSBAR_H__
