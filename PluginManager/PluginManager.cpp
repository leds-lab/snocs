#include "PluginManager.h"

#include <iostream>
#include <cstring>
#include <algorithm>
#include <dlfcn.h>

#include "../NoC/NoC.h"
#include "../Router/Router.h"

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Auxiliar function //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
std::string removeChar(std::string& str, char toRemove) {
    str.erase(std::remove(str.begin(),str.end(),toRemove),str.end());
    return str;
}

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Plugin Loader ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// TODO: Verificar necessidade de passar o contexto de simulação do SystemC sc_curr_simcontext
PluginLoader::PluginLoader(const char *fileName, const char *pluginName)
    : libHandler(0),fileName(fileName),pluginName(pluginName),loaded(false)
{}

PluginLoader::~PluginLoader() {
    if( loaded ) {
        this->unload();
    }
}

bool PluginLoader::load() {
    if(loaded) {
        std::cout << "Plugin " << pluginName << " already loaded." << std::endl;
        return true;
    }
    libHandler = dlopen(fileName,RTLD_NOW);
    if( !libHandler ) {
        loaded = false;
    } else {
        loaded = true;
    }

    return loaded;
}

bool PluginLoader::unload() {
    if(!loaded) {
        std::cout << "Plugin " << pluginName << " not loaded." << std::endl;
        return false;
    }

    return dlclose(libHandler);
}

char* PluginLoader::error() {
    return dlerror();
}

bool PluginLoader::isLoaded() {
    return loaded;
}

void* PluginLoader::loadSymbol(std::string symbol) {
    return dlsym(libHandler,symbol.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Plugin Manager ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
PluginManager::PluginManager()
    : // pluginsDir(pluginsDir), confFile(conf),
      noc(0),
      router(0),routing(0),
      flowControl(0), memory(0),
      priorityGenerator(0),
      pluginsLoaded(false)
{}

PluginManager::~PluginManager() {
    if( pluginsLoaded ) {
        this->deallocateUnits();
        delete noc;
        delete router;
        delete routing;
        delete flowControl;
        delete memory;
        delete priorityGenerator;
    }
}


std::pair<std::string,std::string> PluginManager::parseProperty(std::string line) {

    std::size_t pos = line.find("=");
    std::string key = line.substr(0,pos-1);
    std::string value = line.substr(pos+1);

    key = removeChar(key,' ');
    value = removeChar(value,' ');
    value = removeChar(value,'\n');

    std::string filename = pluginsDir + "/" + value;

    return std::pair<std::string,std::string>(key,filename);
}

bool PluginManager::parseFile(std::string filename,std::string pluginsDir) {
    this->confFile = filename;
    this->pluginsDir = pluginsDir;

    std::string line;

    FILE* file;

    if( (file = fopen(confFile.c_str(),"r")) ) {
        char buff[256];
        while ( std::fgets(buff,sizeof buff,file) != NULL ) {
            line = buff;
            properties.insert( parseProperty(line) );
        }
        fclose(file);
    } else {
        std::cerr << "It was not possible open configuration file in plugin manager" << std::endl;
        return false;
    }

    return true;
}

bool PluginManager::loadPlugins() {

    if( pluginsLoaded ) {
        std::cout << "Plugins already loaded." << std::endl;
        return true;
    } else {
        // Loading NoC
        this->noc = new PluginLoader( this->properties["noc"].c_str(), "NoC" );
        if( !this->noc->load() ) {
            std::cerr << "It was not possible load noc plugin: " << noc->error() << std::endl;
            delete noc;
            return false;
        }
        // Loading router
        this->router = new PluginLoader( this->properties["router"].c_str(), "Router" );
        if( !this->router->load() ) {
            std::cerr << "It was not possible load router plugin: " << router->error() << std::endl;
            delete noc;
            delete router;
            return false;
        }
        // Loading routing
        this->routing = new PluginLoader( this->properties["routing"].c_str(), "Routing" );
        if( !this->routing->load() ) {
            std::cerr << "It was not possible load routing plugin: " << routing->error() << std::endl;
            delete noc;
            delete router;
            delete routing;
            return false;
        }
        // Loading Flow Control
        this->flowControl = new PluginLoader( this->properties["flowcontrol"].c_str(), "FlowControl");
        if( !this->flowControl->load() ) {
            std::cerr << "It was not possible load flow control plugin: " << flowControl->error() << std::endl;
            delete noc;
            delete router;
            delete routing;
            delete flowControl;
            return false;
        }
        // Loading memory
        this->memory = new PluginLoader( this->properties["memory"].c_str(), "Memory" );
        if( !this->memory->load() ) {
            std::cerr << "It was not possible load memory plugin: " << memory->error() << std::endl;
            delete noc;
            delete router;
            delete routing;
            delete flowControl;
            delete memory;
            return false;
        }
        // Loading Priority Generator
        this->priorityGenerator = new PluginLoader(this->properties["prioritygenerator"].c_str(),"PG");
        if( !this->priorityGenerator->load() ) {
            std::cerr << "It was not possible load priority generator plugin: " << priorityGenerator->error() << std::endl;
            delete noc;
            delete router;
            delete routing;
            delete flowControl;
            delete memory;
            delete priorityGenerator;
            return false;
        }

        this->pluginsLoaded = true;
    }

    return true;
}

void PluginManager::output_properties() {
    std::map<std::string,std::string>::iterator it;

    std::cout << "Properties!" << std::endl;
    for( it = properties.begin(); it != properties.end(); it++ ) {
        std::cout << "Key: " << it->first << " - Value: " << it->second << std::endl;
    }
}

void PluginManager::deallocateUnits() {
    std::cout << "Deallocating units from plugins" << std::endl;
    for( unsigned int i = 0; i < allocatedUnits.size(); i++ ) {
        SoCINModule* module = allocatedUnits[i];
        switch( module->moduleType() ) {
            case SoCINModule::NoC: {
                INoC* n = dynamic_cast<INoC* >(module);
                if( n != NULL) {
                    std::cout << "Free - NoC" << std::endl;
                    this->destroyNoC(n);
                }
                break;
            }
            case SoCINModule::Router: {
                IRouter* r = dynamic_cast<IRouter*>(module);
                if( r != NULL ) {
                    std::cout << "Free - Router" << std::endl;
                    this->destroyRouter(r);
                }
                break;
            }
            case SoCINModule::Routing:{
                IRouting* r = dynamic_cast<IRouting*>(module);
                if( r != NULL ) {
                    std::cout << "Free - Routing" << std::endl;
                    this->destroyRouting(r);
                }
                break;
            }
            case SoCINModule::InputFlowControl: {
                IInputFlowControl* ifc = dynamic_cast<IInputFlowControl*>(module);
                if( ifc != NULL ) {
                    std::cout << "Free - IFC" << std::endl;
                    this->destroyInputFlowControl(ifc);
                }
                break;
            }
            case SoCINModule::OutputFlowControl:{
                IOutputFlowControl* ofc = dynamic_cast<IOutputFlowControl*>(module);
                if( ofc != NULL ) {
                    std::cout << "Free - OFC" << std::endl;
                    this->destroyOutputFlowControl(ofc);
                }
                break;
            }
            case SoCINModule::Memory: {
                IMemory* mem = dynamic_cast<IMemory*>(module);
                if( mem != NULL ) {
                    std::cout << "Free - Memory" << std::endl;
                    this->destroyMemory(mem);
                }
                break;
            }
            case SoCINModule::PriorityGenerator: {
                IPriorityGenerator* pg = dynamic_cast<IPriorityGenerator*>(module);
                if( pg != NULL ) {
                    std::cout << "Free - PG" << std::endl;
                    this->destroyPriorityGenerator(pg);
                }
                break;
            }
            default:
                delete module;
                break;
        }
    }
    this->allocatedUnits.clear();
}

INoC* PluginManager::nocInstance(sc_module_name name) {
    if( !pluginsLoaded ) {
        std::cout << "Plugins not loaded to instantiate a noc" << std::endl;
        return NULL;
    }

    create_NoC* new_NoC = (create_NoC*) this->noc->loadSymbol("new_NoC");
    const char* dlsym_error = this->noc->error();
    if( dlsym_error ) {
        std::cerr << "Erro on load symbom of factory creator function - NoC: " << dlsym_error << std::endl;
        return NULL;
    }
    INoC* n = new_NoC(sc_get_curr_simcontext(),name);
    this->allocatedUnits.push_back(n);
    return n;
}

IRouter* PluginManager::routerInstance(sc_module_name name,
                                       unsigned short XID,
                                       unsigned short YID,
                                       unsigned short nPorts)
{
    if( !pluginsLoaded ) {
        std::cout << "Plugins not loaded to instantiate a router" << std::endl;
        return NULL;
    }
    create_Router* new_Router = (create_Router* ) this->router->loadSymbol("new_Router");
    const char* dlsym_error = this->router->error();
    if(dlsym_error) {
       std::cerr << "Error on load symbol of factory creator function - Router: " << dlsym_error << std::endl;
       return NULL;
    }
    IRouter* r = new_Router(sc_get_curr_simcontext(),name,nPorts,XID,YID);
    this->allocatedUnits.push_back(r);
    return r;
}

IRouting* PluginManager::routingInstance(sc_core::sc_module_name name,
                                         unsigned short XID,
                                         unsigned short YID,
                                         unsigned short nPorts)
{
    if( !pluginsLoaded ) {
        std::cout << "Plugins not loaded to instantiate a routing" << std::endl;
        return NULL;
    }
    create_Routing* new_Routing = (create_Routing* ) this->routing->loadSymbol("new_Routing");
    const char* dlsym_error = this->routing->error();
    if(dlsym_error) {
       std::cerr << "Error on load symbol of factory creator function - Routing: " << dlsym_error << std::endl;
       return NULL;
    }
    IRouting* r = new_Routing(sc_get_curr_simcontext(),name,nPorts,XID,YID);
    this->allocatedUnits.push_back(r);
    return r;
}

IInputFlowControl* PluginManager::inputFlowControlInstance(sc_core::sc_module_name name,
                                                           unsigned short XID,
                                                           unsigned short YID,
                                                           unsigned short PORT_ID)
{
    if( !pluginsLoaded ) {
        std::cout << "Plugins not loaded to instantiate a IFC" << std::endl;
        return NULL;
    }
    create_InputFlowControl* new_IFC = (create_InputFlowControl* ) this->flowControl->loadSymbol("new_IFC");
    const char* dlsym_error = this->flowControl->error();
    if(dlsym_error) {
       std::cerr << "Error on load symbol of factory creator function - IFC: " << dlsym_error << std::endl;
       return NULL;
    }
    IInputFlowControl* ifc = new_IFC(sc_get_curr_simcontext(),name,XID,YID,PORT_ID);
    this->allocatedUnits.push_back(ifc);
    return ifc;
}

IOutputFlowControl* PluginManager::outputFlowControlInstance(sc_core::sc_module_name name,
                                                             unsigned short XID,
                                                             unsigned short YID,
                                                             unsigned short PORT_ID,
                                                             unsigned short bufferDepth)
{
    if( !pluginsLoaded ) {
        std::cout << "Plugins not loaded to instantiate a OFC" << std::endl;
        return NULL;
    }
    create_OutputFlowControl* new_OFC = (create_OutputFlowControl* ) this->flowControl->loadSymbol("new_OFC");
    const char* dlsym_error = this->flowControl->error();
    if(dlsym_error) {
       std::cerr << "Error on load symbol of factory creator function - OFC: " << dlsym_error << std::endl;
       return NULL;
    }
    IOutputFlowControl* ofc = new_OFC(sc_get_curr_simcontext(),name,XID,YID,PORT_ID,bufferDepth);
    this->allocatedUnits.push_back(ofc);
    return ofc;
}

IMemory* PluginManager::memoryInstance(sc_core::sc_module_name name,
                                       unsigned short XID,
                                       unsigned short YID,
                                       unsigned short PORT_ID,
                                       unsigned short memSize)
{
    if( !pluginsLoaded ) {
        std::cout << "Plugins not loaded to instantiate a memory" << std::endl;
        return NULL;
    }
    create_Memory* new_Memory = (create_Memory* ) this->memory->loadSymbol("new_Memory");
    const char* dlsym_error = this->memory->error();
    if(dlsym_error) {
       std::cerr << "Error on load symbol of factory creator function - Memory: " << dlsym_error << std::endl;
       return NULL;
    }
    IMemory* mem = new_Memory(sc_get_curr_simcontext(),name,XID,YID,PORT_ID,memSize);
    this->allocatedUnits.push_back(mem);
    return mem;
}

IPriorityGenerator* PluginManager::priorityGeneratorInstance(sc_core::sc_module_name name,
                                                             unsigned short XID,
                                                             unsigned short YID,
                                                             unsigned short PORT_ID,
                                                             unsigned short nPorts)
{
    if( !pluginsLoaded ) {
        std::cout << "Plugins not loaded to instantiate a PG" << std::endl;
        return NULL;
    }
    create_PriorityGenerator* new_PG = (create_PriorityGenerator* ) this->priorityGenerator->loadSymbol("new_PG");
    const char* dlsym_error = this->priorityGenerator->error();
    if(dlsym_error) {
       std::cerr << "Error on load symbol of factory creator function - PG: " << dlsym_error << std::endl;
       return NULL;
    }
    IPriorityGenerator* pg = new_PG(sc_get_curr_simcontext(),name,nPorts,XID,YID,PORT_ID);
    this->allocatedUnits.push_back(pg);
    return pg;
}

void PluginManager::destroyNoC(INoC *n) {
    if( !pluginsLoaded ) {
        return;
    }

    destroy_NoC* delete_NoC = (destroy_NoC* ) this->noc->loadSymbol("delete_NoC");
    const char* dlsym_error = this->noc->error();
    if( dlsym_error ) {
        std::cerr << "Error on load symbol of factory destroy function - NoC: " << dlsym_error << std::endl;
        return;
    }
    delete_NoC(n);
}

void PluginManager::destroyRouter(IRouter *rout) {
    if( !pluginsLoaded ) {
        return;
    }

    destroy_Router* delete_Router = (destroy_Router* ) this->router->loadSymbol("delete_Router");
    const char* dlsym_error = this->router->error();
    if( dlsym_error ) {
        std::cerr << "Error on load symbol of factory destroy function - Router: " << dlsym_error << std::endl;
        return;
    }
    delete_Router(rout);
}

void PluginManager::destroyRouting(IRouting *rout) {
    if( !pluginsLoaded ) {
        return;
    }

    destroy_Routing* delete_Routing = (destroy_Routing* ) this->routing->loadSymbol("delete_Routing");
    const char* dlsym_error = this->routing->error();
    if( dlsym_error ) {
        std::cerr << "Error on load symbol of factory destroy function - Routing: " << dlsym_error << std::endl;
        return;
    }
    delete_Routing(rout);
}

void PluginManager::destroyInputFlowControl(IInputFlowControl *ifc) {
    if( !pluginsLoaded ) {
        return;
    }

    destroy_InputFlowControl* delete_IFC = (destroy_InputFlowControl* ) this->flowControl->loadSymbol("delete_IFC");
    const char* dlsym_error = this->flowControl->error();
    if( dlsym_error ) {
        std::cerr << "Error on load symbol of factory destroy function - IFC: " << dlsym_error << std::endl;
        return;
    }
    delete_IFC(ifc);
}

void PluginManager::destroyOutputFlowControl(IOutputFlowControl *ofc) {
    if( !pluginsLoaded ) {
        return;
    }

    destroy_OutputFlowControl* delete_OFC = (destroy_OutputFlowControl* ) this->flowControl->loadSymbol("delete_OFC");
    const char* dlsym_error = this->flowControl->error();
    if( dlsym_error ) {
        std::cerr << "Error on load symbol of factory destroy function - OFC: " << dlsym_error << std::endl;
        return;
    }
    delete_OFC(ofc);

}

void PluginManager::destroyMemory(IMemory *mem) {
    if( !pluginsLoaded ) {
        return;
    }

    destroy_Memory* delete_Memory = (destroy_Memory* ) this->memory->loadSymbol("delete_Memory");
    const char* dlsym_error = this->memory->error();
    if( dlsym_error ) {
        std::cerr << "Error on load symbol of factory destroy function - Memory: " << dlsym_error << std::endl;
        return;
    }
    delete_Memory(mem);
}

void PluginManager::destroyPriorityGenerator(IPriorityGenerator *pg) {
    if( !pluginsLoaded ) {
        return;
    }

    destroy_PriorityGenerator* delete_PG = (destroy_PriorityGenerator* ) this->priorityGenerator->loadSymbol("delete_PG");
    const char* dlsym_error = this->priorityGenerator->error();
    if( dlsym_error ) {
        std::cerr << "Error on load symbol of factory destroy function - PG: " << dlsym_error << std::endl;
        return;
    }
    delete_PG(pg);
}
