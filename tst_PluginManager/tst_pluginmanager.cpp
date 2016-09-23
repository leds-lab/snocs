#include "../PluginManager/PluginManager.h"

#include "../Memory/Memory.h"

#include <iostream>

int main(int argc, char *argv[]) {

    if( argc < 2 ) {
        std::cerr << "Please specific the plugins folder" << std::endl;
        return -1;
    }

    std::cout << "Plugins folder: " << argv[1] << std::endl;

    PluginManager* pm = new PluginManager("simconf.conf",argv[1]);

    if( pm->parseFile() ) {
        if( pm->loadPlugins() ) {
            std::cout << "Plugins carregados!" << std::endl;
            IMemory* mem = pm->memoryInstance("FIFO",0,0,0,4);
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

    return 0;
}
