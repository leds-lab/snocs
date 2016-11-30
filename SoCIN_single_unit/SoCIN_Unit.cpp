#include "SoCIN_Unit.h"
#include "../Router/Router.h"
#include "../PluginManager/PluginManager.h"

SoCIN_Unit::SoCIN_Unit(sc_module_name mn)
    : INoC_VC( mn, (X_SIZE*Y_SIZE) , NUM_VC)
{
    IRouting* tester = PLUGIN_MANAGER->routingInstance("Tester",0,5);
    if(tester != NULL) {
        if( tester->supportedTopology() != this->topologyType() ) {
            throw std::runtime_error("[SoCIN_Unit] Routing incompatible with the topology");
        }
    }
    // Allocating the number of routers needed
    u_ROUTER.resize(1,NULL);

    // Get router instance
    IRouter* u_R = PLUGIN_MANAGER->routerInstance("SingleRouter",0,numInterfaces,NUM_VC);
    if( u_R == NULL ) {
        throw std::runtime_error("[SoCIN_Unit] -- ERROR: Not possible instantiate a router.");
    }

    u_ROUTER[0] = u_R;

    u_R->i_CLK(i_CLK);
    u_R->i_RST(i_RST);

    IRouter_VC* u_R_VC = dynamic_cast<IRouter_VC *>(u_R);

    for( unsigned short i = 0; i < numInterfaces; i++ ) {
        // Binding common ports
        u_R->i_DATA_IN[i](i_DATA_IN[i]);
        u_R->i_VALID_IN[i](i_VALID_IN[i]);
        u_R->o_RETURN_IN[i](o_RETURN_IN[i]);
        u_R->o_DATA_OUT[i](o_DATA_OUT[i]);
        u_R->o_VALID_OUT[i](o_VALID_OUT[i]);
        u_R->i_RETURN_OUT[i](i_RETURN_OUT[i]);

        // Binding virtual channels if needed
        if( NUM_VC > 1) {
            if( u_R_VC != NULL ) {
                u_R_VC->i_VC_IN[i](i_VC_SELECTOR[i]);
                u_R_VC->o_VC_OUT[i](o_VC_SELECTOR[i]);
            } else {
                throw std::runtime_error("[SoCIN_Unit] -- ERROR: The router instantiated is not a VC router.");
            }
        }
    }
}

SoCIN_Unit::~SoCIN_Unit() {
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
        try {
            return new SoCIN_Unit(moduleName);
        } catch(const std::runtime_error& error) {
            std::cout << "Error to allocate the NoC: " << error.what() << std::endl;
            return NULL;
        }
    }
    SS_EXP void delete_NoC(INoC* noc) {
        delete noc;
    }
}
