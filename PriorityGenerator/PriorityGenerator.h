#ifndef PRIORITYGENERATOR_H
#define PRIORITYGENERATOR_H

#include "../SoCINGlobal.h"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

class PriorityGenerator : public sc_module {
protected:
    unsigned int numPorts;
public:
    // Interfaces
    sc_in_clk                i_CLK;
    sc_in<bool>              i_RST;
    sc_vector<sc_in<bool> >  i_GRANTS;
    sc_vector<sc_out<bool> > o_PRIORITIES;

    unsigned short int XID, YID, PORT_ID;

    virtual const char* moduleTypeName() = 0;

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

typedef PriorityGenerator* create_PriorityGenerator(sc_simcontext*,
                                        sc_module_name,
                                        unsigned int numReqs_Grants,
                                        unsigned short int XID,
                                        unsigned short int YID,
                                        unsigned short int PORT_ID);

typedef void destroy_PriorityGenerator(PriorityGenerator*);

class tst_PG : public sc_module {
private:
    unsigned short int nPorts;
public:
    // Interface
    sc_in_clk                    i_CLK;
    // Signals to stimulus
    sc_signal<bool>              w_RST;
    sc_vector<sc_signal<bool> >  w_GRANTS;
    sc_vector<sc_signal<bool> >  w_PRIORITIES;

    // Design Under Test
    PriorityGenerator* pg;

    // Trace file
    sc_trace_file *tf;

    void tst_stimulus(){
        unsigned short int i;
        // Initialize
        w_RST.write(true);
        for(i = 0; i < nPorts; i++) {
            w_GRANTS[i].write(false);
        }
        wait();

        w_RST.write(false);
        wait();

        // Generating stimulus
        for(i = 0; i < nPorts; i++) {
            w_GRANTS[i].write(true);
            wait();
        }

        wait();

        for(i = nPorts-1; i > 0; i--) {
            w_GRANTS[i].write(false);
            wait();
        }
        w_GRANTS[0].write(false);
        wait();

        sc_stop();

    }

    // Function to destroy DUT instance
    destroy_PriorityGenerator* destroy_PG;

    SC_HAS_PROCESS(tst_PG);
    tst_PG(sc_module_name nm,unsigned short int numPorts,
           create_PriorityGenerator* new_pg, destroy_PriorityGenerator* del)
                : sc_module(nm),nPorts(numPorts), i_CLK("tst_PG_CLK"),
                  w_RST("tst_PG_RESET"), w_GRANTS("tst_PG_GRANTS",numPorts),
                  w_PRIORITIES("tst_PG_PRIORITIES",numPorts)
    {
        destroy_PG = del;

        // Instantiate DUT
        pg = new_pg(sc_get_curr_simcontext(),"PG_DUT",numPorts,0,0,0);
        // Binding DUT with testbench ports and signals
        pg->i_CLK(i_CLK);
        pg->i_RST(w_RST);
        pg->i_GRANTS.bind( w_GRANTS );
        pg->o_PRIORITIES( w_PRIORITIES );

        // Defining testbench stimulus process
        SC_CTHREAD(tst_stimulus,i_CLK.pos());
        sensitive << i_CLK;

        // Creating VCD trace file
        tf = sc_create_vcd_trace_file(pg->moduleTypeName());
        // Signals to trace
        sc_trace(tf, i_CLK, "CLK");
        sc_trace(tf, w_RST, "RST");
        for(unsigned short int i = 0; i < numPorts; i++) {
            char strGrant[12];
            sprintf(strGrant,"GRANT(%u)",i);
            sc_trace(tf, w_GRANTS[i], strGrant);

            char strPriority[15];
            sprintf(strPriority,"PRIORITY(%u)",i);
            sc_trace(tf, w_PRIORITIES[i], strPriority);
        }
    }

    ~tst_PG() {
        // Closing VCD trace file
        sc_close_vcd_trace_file(tf);
        // Deallocating DUT
        destroy_PG(pg);
    }

};

#endif // PRIORITYGENERATOR_H
