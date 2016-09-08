#include "../InputController/InputController.h"
#include "../Routing/Routing.h"

#include <dlfcn.h>
#include <ctime>

/*!
 * \brief sc_main
 * \param argc
 * \param argv [0] Executable folder; [1] Plug-in file; [2] Number of ports
 * \return 0 if success; -1 otherwise
 */
int sc_main(int argc, char *argv[]) {

    for(int i = 0; i < argc; i++) {
        std::cout << "Argv[" << i << "]: " << argv[i] << std::endl;
    }
    if( argc < 3 ) {
        std::cerr << "Please specific the Routing plugin filename and number of ports"
                     "\nFormat: ./tst_inputcontroller </dir/of/pg_plugin> NPORTS" << std::endl;
        return -1;
    }

    // Get plugin filename
    char* pluginFilename = argv[1];
    std::cout << "Plugin Filename: " << pluginFilename << std::endl;
    ///// Loading library of Routing plugin
    void* lib_handle;
    lib_handle = dlopen(pluginFilename,RTLD_NOW);
    if( !lib_handle ) {
        std::cerr << "Error to open plugin " << pluginFilename <<": " << dlerror() << std::endl;
        return -1;
    }

    create_Routing* new_Routing_t = (create_Routing*) dlsym(lib_handle,"new_Routing");
    const char* dlsym_error = dlerror();
    if( dlsym_error ) {
        std::cout << dlsym_error << std::endl;
        std::cerr << "Error on load symbol of factory creator function - Routing: " << pluginFilename << std::endl;
        return -1;
    }

    destroy_Routing* delete_Routing_t = (destroy_Routing*) dlsym(lib_handle,"delete_Routing");
    dlsym_error = dlerror();
    if( dlsym_error ) {
        std::cerr << "Error on load symbol of factory destroy function - Routing: " << pluginFilename << std::endl;
        return -1;
    }

    unsigned short nPorts, XID, YID, PORT_ID;
    // Get number of ports
    nPorts = atoi(argv[2]);
    XID = 0;
    YID = 0;
    PORT_ID = 0; // LOCAL port

    printf("\nNumber of Ports: %u",nPorts);

    // System Clock - 10ns
    sc_clock w_CLK("CLK",100,SC_NS);

    ////////////// Instantiate system components //////////////
    // Routing
    IRouting* routing = new_Routing_t(sc_get_curr_simcontext(),"Routing",nPorts,XID,YID);
    // Tester (Driver) - testbench
    tst_InputController* ic = new tst_InputController("Testbench",routing,nPorts,XID,YID,PORT_ID);
    ic->i_CLK(w_CLK);

    clock_t t = clock();
    // Start simulation
    sc_start();
    t = clock() - t;
    printf("\n\nSimulated in %ld ms(%f secs)\n",t,((float)t)/CLOCKS_PER_SEC);

    delete ic;
    delete_Routing_t(routing);
    dlclose(lib_handle);

    return 0;
}
