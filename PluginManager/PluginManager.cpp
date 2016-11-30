#include "PluginManager.h"

#include <iostream>
#include <cstdio>
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
PluginLoader::PluginLoader(std::string fileName, std::string pluginName)
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

    libHandler = dlopen(fileName.c_str(),RTLD_NOW);
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
    : noc(0),
      router(0),routing(0),
      flowControl(0), memory(0),
      priorityGenerator(0),
      pluginsLoaded(false)
{
    properties["noc"] = "";
    properties["router"] = "";
    properties["routing"] = "";
    properties["flowcontrol"] = "";
    properties["memory"] = "";
    properties["prioritygenerator"] = "";
}

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

void PluginManager::parseProperty(char* line) {

    char key[30];
    char value[30];
    sscanf(line,"%s = %s",key,value);

    char filename[256];
    sprintf(filename,"%s/%s",PLUGINS_DIR,value);

    if( properties.find(key) != properties.end() ) {
        properties[key] = filename;
    }
}

bool PluginManager::parseFile() {

    FILE* file;

    char strConfFile[256];
    sprintf(strConfFile,"%s/%s",WORK_DIR,CONF_FILE);
    if( (file = fopen(strConfFile,"r")) ) {
        char buff[256];
        while ( fgets(buff,sizeof buff,file) != NULL ) {
            parseProperty(buff);
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
        this->noc = new PluginLoader( this->properties["noc"], "NoC" );
        if( !this->noc->load() ) {
            std::cerr << "It was not possible load noc plugin: " << noc->error() << std::endl;
            delete noc;
            return false;
        }
        // Loading router
        this->router = new PluginLoader( this->properties["router"], "Router" );
        if( !this->router->load() ) {
            std::cerr << "It was not possible load router plugin: " << router->error() << std::endl;
            delete noc;
            delete router;
            return false;
        }
        // Loading routing
        this->routing = new PluginLoader( this->properties["routing"], "Routing" );
        if( !this->routing->load() ) {
            std::cerr << "It was not possible load routing plugin: " << routing->error() << std::endl;
            delete noc;
            delete router;
            delete routing;
            return false;
        }
        // Loading Flow Control
        this->flowControl = new PluginLoader( this->properties["flowcontrol"], "FlowControl");
        if( !this->flowControl->load() ) {
            std::cerr << "It was not possible load flow control plugin: " << flowControl->error() << std::endl;
            delete noc;
            delete router;
            delete routing;
            delete flowControl;
            return false;
        }
        // Loading memory
        this->memory = new PluginLoader( this->properties["memory"], "Memory" );
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
        this->priorityGenerator = new PluginLoader(this->properties["prioritygenerator"],"PG");
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
            case SoCINModule::TNoC: {
                INoC* n = dynamic_cast<INoC* >(module);
                if( n != NULL) {
                    this->destroyNoC(n);
                }
                break;
            }
            case SoCINModule::TRouter: {
                IRouter* r = dynamic_cast<IRouter*>(module);
                if( r != NULL ) {
                    this->destroyRouter(r);
                }
                break;
            }
            case SoCINModule::TRouting:{
                IRouting* r = dynamic_cast<IRouting*>(module);
                if( r != NULL ) {
                    this->destroyRouting(r);
                }
                break;
            }
            case SoCINModule::TInputFlowControl: {
                IInputFlowControl* ifc = dynamic_cast<IInputFlowControl*>(module);
                if( ifc != NULL ) {
                    this->destroyInputFlowControl(ifc);
                }
                break;
            }
            case SoCINModule::TOutputFlowControl:{
                IOutputFlowControl* ofc = dynamic_cast<IOutputFlowControl*>(module);
                if( ofc != NULL ) {
                    this->destroyOutputFlowControl(ofc);
                }
                break;
            }
            case SoCINModule::TMemory: {
                IMemory* mem = dynamic_cast<IMemory*>(module);
                if( mem != NULL ) {
                    this->destroyMemory(mem);
                }
                break;
            }
            case SoCINModule::TPriorityGenerator: {
                IPriorityGenerator* pg = dynamic_cast<IPriorityGenerator*>(module);
                if( pg != NULL ) {
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
    std::cout << "Units deallocated!" << std::endl;
}

INoC* PluginManager::nocInstance(sc_module_name name) {
    if( !pluginsLoaded ) {
        std::cout << "Plugins not loaded to instantiate a noc" << std::endl;
        return NULL;
    }

    create_NoC* new_NoC = (create_NoC*) this->noc->loadSymbol("new_NoC");
    const char* dlsym_error = this->noc->error();
    if( dlsym_error ) {
        std::cerr << "Error on load symbom of factory creator function - NoC: " << dlsym_error << std::endl;
        return NULL;
    }
    INoC* n = new_NoC(sc_get_curr_simcontext(),name);
    if(n) {
        this->allocatedUnits.push_back(n);
    }
    return n;
}

IRouter* PluginManager::routerInstance(sc_module_name name,
                                       unsigned short ROUTER_ID,
                                       unsigned short nPorts,
                                       unsigned short nVirtualChannels)
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
    IRouter* r = new_Router(sc_get_curr_simcontext(),name,nPorts,nVirtualChannels,ROUTER_ID);
    if(r) {
        this->allocatedUnits.push_back(r);
    }
    return r;
}

IRouting* PluginManager::routingInstance(sc_core::sc_module_name name,
                                         unsigned short ROUTER_ID,
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
    IRouting* r = new_Routing(sc_get_curr_simcontext(),name,nPorts,ROUTER_ID);
    if(r) {
        this->allocatedUnits.push_back(r);
    }
    return r;
}

IInputFlowControl* PluginManager::inputFlowControlInstance(sc_core::sc_module_name name,
                                                           unsigned short ROUTER_ID,
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
    IInputFlowControl* ifc = new_IFC(sc_get_curr_simcontext(),name,ROUTER_ID,PORT_ID);
    if(ifc) {
        this->allocatedUnits.push_back(ifc);
    }
    return ifc;
}

IOutputFlowControl* PluginManager::outputFlowControlInstance(sc_core::sc_module_name name,
                                                             unsigned short ROUTER_ID,
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
    IOutputFlowControl* ofc = new_OFC(sc_get_curr_simcontext(),name,ROUTER_ID,PORT_ID,bufferDepth);
    if( ofc ) {
        this->allocatedUnits.push_back(ofc);
    }
    return ofc;
}

IMemory* PluginManager::memoryInstance(sc_core::sc_module_name name,
                                       unsigned short ROUTER_ID,
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
    IMemory* mem = new_Memory(sc_get_curr_simcontext(),name,ROUTER_ID,PORT_ID,memSize);
    if(mem) {
        this->allocatedUnits.push_back(mem);
    }
    return mem;
}

IPriorityGenerator* PluginManager::priorityGeneratorInstance(sc_core::sc_module_name name,
                                                             unsigned short ROUTER_ID,
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
    IPriorityGenerator* pg = new_PG(sc_get_curr_simcontext(),name,nPorts,ROUTER_ID,PORT_ID);
    if(pg) {
        this->allocatedUnits.push_back(pg);
    }
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
