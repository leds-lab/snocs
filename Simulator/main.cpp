#include "../NoC/NoC.h"
#include "../Router/Router.h"

#include "../PluginManager/PluginManager.h"

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

int sc_main(int argc, char* argv[]) {

    std::cout << " >>> Main - SimContext: " << static_cast<void*>(sc_curr_simcontext)
              << ", pAddr: " << &sc_curr_simcontext << std::endl;

    X_SIZE = 3;
    Y_SIZE = 3;

//    std::cout << " >>> Main - SimContext: " << static_cast<void*>(sc_get_curr_simcontext())
//              << ", PointerAddrr: " << sc_get_curr_simcontext() << std::endl;

    if( PLUGIN_MANAGER->parseFile() ) {
        if( PLUGIN_MANAGER->loadPlugins() ) {
            std::cout << "Plugins carregados!" << std::endl;
            INoC* u_NETWORK = PLUGIN_MANAGER->nocInstance("NoC");
            std::cout << "NoC instanciada!" << std::endl;

            unsigned short i, numRouters = u_NETWORK->u_ROUTER.size();

            std::cout << "NoC: " << u_NETWORK->moduleName() << ", Num routers: " << numRouters << std::endl;

            for( i = 0; i < numRouters; i++) {
                IRouter* router = u_NETWORK->u_ROUTER[i];
//                std::cout << "R[" << i << "]: " << router->moduleName();
//                std::cout << ", XIN.size: " << router->u_XIN.size() << std::endl;
//                router->u_XIN.size();
            }
        } else {
            std::cout << "Falha ao carregar plugins ;-(" << std::endl;
        }
    } else {
        std::cout << "Parse - ops" << std::endl;
    }

    return 0;
}
