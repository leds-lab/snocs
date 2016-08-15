#include "../Arbiter/Arbiter.h"

#include <dlfcn.h>
#include <ctime>

int sc_main(int argc, char *argv[]) {

    for(int i = 0; i < argc; i++) {
        std::cout << "Argv[" << i << "]: " << argv[i] << std::endl;
    }
    if( argc < 3 ) {
        std::cerr << "Please specific PG plugin filename and number of ports"
                     "\nFormat: ./Arb_Tester </dir/of/pg_plugin> NPORTS" << std::endl;
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
    std::cout << dlsym_error << std::endl;
    if( dlsym_error ) {
        std::cerr << "Error on load symbol of factory creator function - PG: " << pluginFilename << std::endl;
        return -1;
    }

    destroy_PriorityGenerator* delete_PG_t = (destroy_PriorityGenerator*) dlsym(lib_handle,"delete_PG");
    dlsym_error = dlerror();
    if( dlsym_error ) {
        std::cerr << "Error on load symbol of factory destroy function - PG: " << pluginFilename << std::endl;
        return -1;
    }

    unsigned short nPorts, XID, YID, PORT_ID;
    // Get number of ports
    nPorts = atoi(argv[2]);
    XID = YID = PORT_ID = 0;

    std::cout << "\nNumber of Ports: " << nPorts << std::endl;

    // System Clock - 10ns
    sc_clock w_CLK("CLK",10,SC_NS);

    ////////////// Instantiate system components //////////////
    // Priority Generator
    IPriorityGenerator* pg = new_PG_t(sc_get_curr_simcontext(),"PG",nPorts,XID,YID,PORT_ID);
    // Programmable Priority Encoder
    ProgrammablePriorityEncoder* ppe = new ProgrammablePriorityEncoder("PPE",nPorts,XID,YID,PORT_ID);
    // Design Under Test
    Arbiter* arb = new Arbiter("Arbiter",pg,ppe,nPorts,XID,YID,PORT_ID);
    // Tester (Driver) - testbench
    tst_Arbiter* tb = new tst_Arbiter("Testbench",arb,nPorts);
    tb->i_CLK(w_CLK);

    clock_t t = clock();
    // Start simulation
    sc_start();
    t = clock() - t;
    printf("\n\nSimulated in %ld (%f secs)\n",t,((float)t)/CLOCKS_PER_SEC);

    delete tb;
    delete_PG_t(pg);
    dlclose(lib_handle);


    return 0;
}
