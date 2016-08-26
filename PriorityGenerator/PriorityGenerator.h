/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : IPriorityGenerator
FILE   : PriorityGenerator.h
--------------------------------------------------------------------------------
DESCRIPTION: It is a interface class to implement priority generators
which determines the next priority levels by implementing a specific grant policy.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 10/08/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef PRIORITYGENERATOR_H
#define PRIORITYGENERATOR_H

#include "../SoCINGlobal.h"
#include <ctime>

/////////////////////////////////////////////////////////////
/// Interface for Priority Generators (PGs)
/////////////////////////////////////////////////////////////
/*!
 * \brief The IPriorityGenerator class is an interface
 * (abstract class) for Priority Generators of the Arbiters.
 */
class IPriorityGenerator : public SoCINModule {
protected:
    unsigned short int numPorts;
public:
    // Interfaces
    sc_in_clk                i_CLK;
    sc_in<bool>              i_RST;
    sc_vector<sc_in<bool> >  i_GRANTS;
    sc_vector<sc_out<bool> > o_PRIORITIES;

    unsigned short int XID, YID, PORT_ID;

    // Constructor
    IPriorityGenerator(sc_module_name mn,
                      unsigned short int numReqs_Grants,
                      unsigned short int XID,
                      unsigned short int YID,
                      unsigned short int PORT_ID)
        : SoCINModule(mn),numPorts(numReqs_Grants), i_CLK("PG_CLK"),
          i_RST("PG_RESET"),i_GRANTS("PG_GRANTS",numReqs_Grants),
          o_PRIORITIES("PG_PRIORITIES",numReqs_Grants),XID(XID),
          YID(YID),PORT_ID(PORT_ID) {}

    ~IPriorityGenerator() = 0;
};
inline IPriorityGenerator::~IPriorityGenerator(){}
/////////////////////////////////////////////////////////////
/// End Priority Generator Interface
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
/// Typedefs for Factory Methods of concrete PGs
/////////////////////////////////////////////////////////////
/*!
 * \brief create_PriorityGenerator Typedef for instantiate a
 * Priority Generator
 * \param sc_simcontext A pointer of simulation context (required for correct plugins use)
 * \param sc_module_name Name for the module to be instantiated
 * \param numReqs_Grants Number of input Requests and output Grants ports
 * \param XID Column identifier of the router in the network
 * \param YID Row identifier of the router in the network
 * \param PORT_ID Port identifier of the router
 * \return A method for instantiate a Priority Generator
 */
typedef IPriorityGenerator* create_PriorityGenerator(sc_simcontext*,
                                        sc_module_name,
                                        unsigned short int numReqs_Grants,
                                        unsigned short int XID,
                                        unsigned short int YID,
                                        unsigned short int PORT_ID);

/*!
 * \brief destroy_PriorityGenerator Typedef for deallocating a
 * Priority Generator
 */
typedef void destroy_PriorityGenerator(IPriorityGenerator*);
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
///  Testbench
/////////////////////////////////////////////////////////////
/*!
 * \brief The tst_PG class is a self-contained testbench for PGs
 */
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
    IPriorityGenerator* pg;

    // Trace file
    sc_trace_file *tf;

    void p_stimulus(){
        srand(time(NULL));
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
            if( i > 0 ) {
                w_GRANTS[i-1].write(false);
            }
            w_GRANTS[i].write(true);
            wait();
        }

        w_GRANTS[nPorts-1].write(false);
        wait();

        for(i = 0; i < nPorts * 3; i++) {
            unsigned short int sorted = rand() % nPorts;
            w_GRANTS[sorted].write(true);
            wait();
            w_GRANTS[sorted].write(false);
        }
        wait();

        for(i = nPorts-1; i < nPorts; i--) {
            if(i < nPorts-1) {
                w_GRANTS[i+1].write(false);
            }
            w_GRANTS[i].write(true);
            wait();
        }

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
        SC_CTHREAD(p_stimulus,i_CLK.pos());
        sensitive << i_CLK;

        // Creating VCD trace file
        tf = sc_create_vcd_trace_file(pg->moduleName());
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

/////////////////////////////////////////////////////////////
///  End testbench
/////////////////////////////////////////////////////////////


#endif // PRIORITYGENERATOR_H
