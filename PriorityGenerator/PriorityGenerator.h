#ifndef PRIORITYGENERATOR_H
#define PRIORITYGENERATOR_H

#include "prioritygenerator_global.h"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

class PG_SHARED_EXPORT PriorityGenerator : public sc_module {
protected:
    unsigned int numPorts;
public:
    // Interfaces
    sc_in_clk                i_CLK;
    sc_in<bool>              i_RST;
    sc_vector<sc_in<bool> >  i_GRANTS;
    sc_vector<sc_out<bool> > o_PRIORITIES;

    unsigned short int XID, YID, PORT_ID;

    // Constructor
    PriorityGenerator(sc_module_name mn,
                      unsigned int numReqs_Grants,
                      unsigned short int XID,
                      unsigned short int YID,
                      unsigned short int PORT_ID)
        : sc_module(mn),numPorts(numReqs_Grants), i_CLK("PG_CLK"),
          i_RST("PG_RESET"),i_GRANTS("PG_GRANTS",numReqs_Grants),
          o_PRIORITIES("PG_PRIORITIES",numReqs_Grants),XID(XID),
          YID(YID),PORT_ID(PORT_ID) {}

    // Destructor
    virtual ~PriorityGenerator() = 0;
};

inline PriorityGenerator::~PriorityGenerator() {}

typedef PriorityGenerator* new_PriorityGenerator(sc_simcontext*,
                                        sc_module_name,
                                        unsigned int numReqs_Grants,
                                        unsigned short int XID,
                                        unsigned short int YID,
                                        unsigned short int PORT_ID);

typedef void delete_PriorityGenerator(PriorityGenerator*);

#endif // PRIORITYGENERATOR_H
