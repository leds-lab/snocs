#ifndef __PG_RANDOM_H__
#define __PG_RANDOM_H__

#include "../src/PriorityGenerator.h"
#include <random>
/*!
 * \brief The PG_Random class implements a random
 * priority generator that randomizes the priorities in
 * each arbitration
 */
class PG_Random : public IPriorityGenerator {
public:

    std::default_random_engine randomGenerator;
    std::uniform_int_distribution<int> uniformRandom;

    // Module's processes
    void p_OUTPUTS();

    SC_HAS_PROCESS(PG_Random);
    PG_Random(sc_module_name mn,
              unsigned short int numReqs_Grants,
              unsigned short int ROUTER_ID,
              unsigned short int PORT_ID);

    ModuleType moduleType() const { return  SoCINModule::TPriorityGenerator; }
    const char* moduleName() const { return "PG_Random"; }
};


#endif // __PG_RANDOM_H__
