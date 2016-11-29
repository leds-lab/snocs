#ifndef __ROUTING_RING_H__
#define __ROUTING_RING_H__

#include "../Routing/Routing.h"

class Routing_Ring : public IRouting {
protected:
    unsigned char REQ_NONE;
    unsigned char REQ_LOCAL;
    unsigned char REQ_CLOCKWISE;     // left
    unsigned char REQ_ANTICLOCKWISE; // right
public:

    // Module's process
    void p_REQUEST();

    SC_HAS_PROCESS(Routing_Ring);
    Routing_Ring(sc_module_name mn,
                 unsigned short nPorts,
                 unsigned short ROUTER_ID);

    const char* moduleName() const { return "Routing_Ring"; }
};

#endif // __ROUTING_RING_H__
