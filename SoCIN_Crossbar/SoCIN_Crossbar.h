#ifndef __SOCIN_CROSSBAR_H__
#define __SOCIN_CROSSBAR_H__

#include "../NoC/NoC.h"

class SoCIN_Crossbar : public INoC_VC {

public:
    // Inherits interface from INoC_VC

    SC_HAS_PROCESS(SoCIN_Crossbar);
    SoCIN_Crossbar(sc_module_name mn);

    const char* moduleName() const { return "Crossbar"; }

    ~SoCIN_Crossbar();
};

#endif // SOCIN_CROSSBAR_H
