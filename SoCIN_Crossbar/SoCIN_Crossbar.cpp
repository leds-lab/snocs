#include "SoCIN_Crossbar.h"
#include "../Router/Router.h"
#include "../PluginManager/PluginManager.h"

SoCIN_Crossbar::SoCIN_Crossbar(sc_module_name mn)
    : INoC_VC( mn, (X_SIZE*Y_SIZE) , NUM_VC)
{
    // Allocating the number of routers needed
    u_ROUTER.resize(1,NULL);

    // Get router instance
    IRouter* u_R = PLUGIN_MANAGER->routerInstance("ParIS_Crossbar",0,0,(X_SIZE*Y_SIZE),NUM_VC);
    if( u_R == NULL ) {
        std::cout << "\n\t[SoCINfp_VC] -- ERROR: Not possible instantiate a router." << std::endl;
        return;
    }

    u_ROUTER[0] = u_R;

    u_R->i_CLK(i_CLK);
    u_R->i_RST(i_RST);

    IRouter_VC* u_R_VC = dynamic_cast<IRouter_VC *>(u_R);

    for( unsigned short i = 0; i < u_R->numPorts; i++ ) {
        // Binding common ports
        u_R->i_DATA_IN[i](i_DATA_IN[i]);
        u_R->i_VALID_IN[i](i_VALID_IN[i]);
        u_R->o_RETURN_IN[i](o_RETURN_IN[i]);
        u_R->o_DATA_OUT[i](o_DATA_OUT[i]);
        u_R->o_VALID_OUT[i](o_VALID_OUT[i]);
        u_R->i_RETURN_OUT[i](i_RETURN_OUT[i]);

        // Binding virtual channels if needed
        if( NUM_VC > 1 ) {
            u_R_VC->i_VC_IN[i](i_VC_SELECTOR[i]);
            u_R_VC->o_VC_OUT[i](o_VC_SELECTOR[i]);
        }
    }
}

SoCIN_Crossbar::~SoCIN_Crossbar() {
    u_ROUTER.clear();
}


////////////////////////////////////////////////////////////////////////////////
/*!
 * Factory Methods to instantiation and deallocation
 */
extern "C" {
    SS_EXP INoC* new_NoC(sc_simcontext* simcontext,sc_module_name moduleName) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new SoCIN_Crossbar(moduleName);
    }
    SS_EXP void delete_NoC(INoC* noc) {
        delete noc;
    }
}
