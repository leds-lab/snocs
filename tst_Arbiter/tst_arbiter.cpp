//#define USE_TEST

#ifdef USE_TEST
#include "../src/Arbiter.h"

// TODO: mudar para usar PluginLoader
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
        std::cerr << "Please specific the PG plugin filename and number of ports"
                     "\nFormat: ./tst_Arbiter </dir/of/pg_plugin> NPORTS" << std::endl;
        return -1;
    }

    // Get plugin filename
    char* pluginFilename = argv[1];
    std::cout << "Plugin Filename: " << pluginFilename << std::endl;
    ///// Loading library of PG plugin
    void* lib_handle;
    lib_handle = dlopen(pluginFilename,RTLD_NOW);
    if( !lib_handle ) {
        std::cerr << "Error to open plugin " << pluginFilename <<": " << dlerror() << std::endl;
        return -1;
    }

    create_PriorityGenerator* new_PG_t = (create_PriorityGenerator*) dlsym(lib_handle,"new_PG");
    const char* dlsym_error = dlerror();
    if( dlsym_error ) {
        std::cout << dlsym_error << std::endl;
        std::cerr << "Error on load symbol of factory creator function - PG: " << pluginFilename << std::endl;
        return -1;
    }

    destroy_PriorityGenerator* delete_PG_t = (destroy_PriorityGenerator*) dlsym(lib_handle,"delete_PG");
    dlsym_error = dlerror();
    if( dlsym_error ) {
        std::cerr << "Error on load symbol of factory destroy function - PG: " << pluginFilename << std::endl;
        return -1;
    }

    unsigned short nPorts, ROUTER_ID, PORT_ID;
    // Get number of ports
    nPorts = atoi(argv[2]);
    ROUTER_ID = PORT_ID = 0;

    printf("\nNumber of Ports: %u",nPorts);

    // System Clock - 10ns
    sc_clock w_CLK("CLK",100,SC_NS);

    ////////////// Instantiate system components //////////////
    // Priority Generator
    IPriorityGenerator* pg = new_PG_t(sc_get_curr_simcontext(),"PG",nPorts,ROUTER_ID,PORT_ID);
    // Design Under Test
    IArbiter* arb = new DistributedArbiter("Arbiter",nPorts,pg,ROUTER_ID,PORT_ID);
    // Tester (Driver) - testbench
    tst_Arbiter* tb = new tst_Arbiter("Testbench",arb,nPorts);
    tb->i_CLK(w_CLK);

    clock_t t = clock();
    // Start simulation
    sc_start();
    t = clock() - t;
    printf("\n\nSimulated in %ld ms(%f secs)\n",t,((float)t)/CLOCKS_PER_SEC);

    delete tb;
    delete_PG_t(pg);
    dlclose(lib_handle);

    return 0;
}
#endif
