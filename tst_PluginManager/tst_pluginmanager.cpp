#include "../src/PluginManager.h"

#include "../src/NoC.h"

#include <iostream>
using namespace std;

int sc_main(int argc, char *argv[]) {
/*
    if( argc < 2 ) {
        std::cerr << "Please specific the plugins folder" << std::endl;
        return -1;
    }

    std::cout << "Plugins folder: " << argv[1] << std::endl;

    PluginManager* pm = new PluginManager();

    if( pm->parseFile() ) {
        if( pm->loadPlugins() ) {
            std::cout << "Plugins carregados!" << std::endl;
            IMemory* mem = pm->memoryInstance("FIFO",0,0,4);
            if(mem) {
                std::cout << mem->moduleName() << std::endl;
            }
        } else {
            std::cout << "Falha ao carregar plugins ;-(" << std::endl;
        }
    } else {
        std::cout << "Parse - ops" << std::endl;
    }
    delete pm;
*/
    PluginLoader* pl = new PluginLoader("../plugins/noc_SoCIN.dll","NoC");
    if( pl->load() ) {
        cout << "Carregado" << endl;
        create_NoC* new_NoC = (create_NoC*) pl->loadSymbol("new_NoC");
        if(new_NoC == NULL) {
            cout << "Simbolo nao carregado!" << endl;
            const char* dlsym_error = pl->error();
            cout << "Conteudo de error: " << dlsym_error << endl;
        } else {
            cout << "Simbolo carregado!" << endl;
            INoC* n = new_NoC(sc_get_curr_simcontext(),"NoC");
            if(n) {
                cout << "Basename of NoC: " << n->basename() << endl;
            }
        }
        delete pl;
    } else {
        cout << "Nao carregou" << endl;
    }



    return 0;
}
