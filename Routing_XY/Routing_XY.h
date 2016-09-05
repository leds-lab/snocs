#ifndef ROUTING_XY_H
#define ROUTING_XY_H

#include "../Routing/Routing.h"

class Routing_XY : public IRouting {
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
