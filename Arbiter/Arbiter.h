#ifndef ARBITER_H
#define ARBITER_H

#include <systemc>
#include "../PriorityGenerator/PriorityGenerator.h"
#include "../ProgrammablePriorityEncoder/ProgrammablePriorityEncoder.h"

using namespace sc_core;
using namespace sc_dt;

/*!
 * \brief The Arbiter class represents the Arbiter entity used by OC (Output Controller)
 */
class Arbiter : public sc_module {
    unsigned short nPorts;
public:
    // System interfaces
    sc_in<bool> i_CLK; // Clock
    sc_in<bool> i_RST; // Reset

    // Arbitration interfaces
    sc_vector<sc_in<bool> >  i_REQUEST; // Requests
    sc_vector<sc_out<bool> > o_GRANT;   // Grants
    sc_out<bool>             o_IDLE;    // Status

    // Internal signals
    sc_vector<sc_signal<bool> > w_PRIORITY; // Priorities

    // Internal data structures
    unsigned short XID, YID, PORT_ID;

    // Internal Units - Components
    IPriorityGenerator* u_PG;
    ProgrammablePriorityEncoder* u_PPE;

    SC_HAS_PROCESS(Arbiter);
    Arbiter(sc_module_name mn, IPriorityGenerator* pg, ProgrammablePriorityEncoder* ppe, unsigned short nPorts,
            unsigned short XID, unsigned short YID, unsigned short PORT_ID);
};


/*!
 * \brief The tst_Arbiter class represents the testbench of Arbiter module
 */
class tst_Arbiter : public sc_module {
    unsigned short nPorts;
public:
    // Interface
    sc_in_clk       i_CLK;
    // Signals to stimulus
    sc_signal<bool> w_RST;
    sc_vector<sc_signal<bool> > w_REQUEST;
    sc_vector<sc_signal<bool> > w_GRANT;
    sc_signal<bool>             w_IDLE;

    // Design Under Test
    Arbiter* arb;

    // Trace file
    sc_trace_file* tf;

    void p_stimulus();

    SC_HAS_PROCESS(tst_Arbiter);
    tst_Arbiter(sc_module_name mn,Arbiter* arb,unsigned short nPorts);

    ~tst_Arbiter();
};

#endif // ARBITER_H
