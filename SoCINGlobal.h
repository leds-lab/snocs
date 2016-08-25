// Detect Windows OS
#ifdef _WIN32
#    define SS_EXP __declspec(dllexport)
#    define SS_IMP __declspec(dllimport)
// Unix-based - Linux and OS X
#elif __unix__ || __APPLE__
#    define SS_EXP __attribute__((visibility("default")))
#    define SS_IMP __attribute__((visibility("default")))
#else
#    error "Unknown_compiler"
#endif

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

/*!
 * \brief The SoCINModule class is the interface for all
 * SoCIN modules
 */
class SoCINModule : public sc_module {
public:

    enum ModuleType { Router = 1,
                      Routing,
                      FlowControl,
                      InputFlowControl,
                      OutputFlowControl,
                      Arbiter,
                      Buffer,
                      PriorityGenerator};

    virtual ModuleType moduleType() const = 0;
    virtual const char* moduleName() const = 0;

    SoCINModule(sc_module_name mn) : sc_module (mn) {}

    virtual ~SoCINModule() = 0;
};

inline SoCINModule::~SoCINModule() {}
