#ifndef __SOCIN_UNIT_H__
#define __SOCIN_UNIT_H__

#include "../NoC/NoC.h"

class SoCIN_Unit : public INoC_VC {

public:
    // Inherits interface from INoC_VC

    SC_HAS_PROCESS(SoCIN_Unit);
    SoCIN_Unit(sc_module_name mn);

    const char* moduleName() const { return "SoCIN_SingleUnit"; }

    ~SoCIN_Unit();
};

#endif // __SOCIN_UNIT_H__
