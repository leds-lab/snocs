#include "../src/PriorityGenerator.h"

#include <dlfcn.h>
#include <ctime>

/*!
 * \brief sc_main
 * \param argc
 * \param argv [0] Executable folder; [1] Plug-in file
 * \return
 */

int sc_main(int argc, char *argv[]) {

    for(int i = 0; i < argc; i++) {
        std::cout << "Argv[" << i << "]: " << argv[i] << std::endl;
    }
    if( argc < 2 ) {
        std::cerr << "Please specific plugin filename" << std::endl;
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

    // System Clock - 10ns
    sc_clock    w_CLK("CLK",10,SC_NS);

    /*
     * Parameters:
     * @1 : Module Name
     * @2 : Number of ports of PG - Grants and Requests
     * @3 : Creator function (factory) from PG library
     * @4 : Destroy function (factory) from PG library
     *
     */
    tst_PG* tst = new tst_PG("PG_Testbench",4,new_PG_t,delete_PG_t);
    tst->i_CLK(w_CLK);

    clock_t t = clock();
    // Start simulation
    sc_start();
    t = clock() - t;
    printf("Simulated in %ld (%f secs)\n",t,((float)t)/CLOCKS_PER_SEC);

    delete tst;
    dlclose(lib_handle);

    return 0;
}
