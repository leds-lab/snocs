/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : ProgramablePriorityEncoder
FILE   : ProgramablePriorityEncoder.h
--------------------------------------------------------------------------------
DESCRIPTION: Programmable priority encoder that receives a set of requests and
priorities, and, based on the current priorities, schedules one of the pending
requests by giving it a grant. It is composed by "N" arbitration cells
interconnected in a ripple loop (wrap-around connection), implemented by signals
which notify the next cell if some of the previous cells has already granted a
request. This entity also include a register which holds the last granting until
the granted request return to 0. A new grant can only be given after the arbiter
returns to the idle state.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 10/08/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/

#ifndef PROGRAMMABLEPRIORITYENCODER_H
#define PROGRAMMABLEPRIORITYENCODER_H

#include "../SoCINGlobal.h"

class ProgrammablePriorityEncoder : public SoCINModule {
protected:
    unsigned short int nPorts;
public:

    // System signals
    sc_in<bool> i_CLK;                       // Clock
    sc_in<bool> i_RST;                       // Reset

    // Interfaces
    sc_vector<sc_in<bool> >  i_REQUEST;      // Requests
    sc_vector<sc_in<bool> >  i_PRIORITY;     // Priorities
    sc_vector<sc_out<bool> > o_GRANT;        // Grants
    sc_out<bool>             o_IDLE;         // Status

    // Internal signals
    sc_vector<sc_signal<bool> > w_IMED_IN;   // Some of the previous cell granted a req.
    sc_vector<sc_signal<bool> > w_IMED_OUT;  // A grant was already given
    sc_vector<sc_signal<bool> > w_GRANT;     // Grant signals
    sc_vector<sc_signal<bool> > r_GRANT;     // Registered grant signals
    sc_signal<bool>             w_IDLE;      // Signal for the idle output

    // Internal data structures
    unsigned short int XID, YID, PORT_ID;

    // Module's processes
    void p_IMED_IN();
    void p_IMED_OUT();
    void p_GRANT();
    void p_GRANT_REG();
    void p_IDLE();
    void p_OUTPUTS();

    SC_HAS_PROCESS(ProgrammablePriorityEncoder);
    ProgrammablePriorityEncoder(sc_module_name mn,
                                unsigned int short nPorts,
                                unsigned int short XID,
                                unsigned int short YID,
                                unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::PriorityEncoder; }
    const char* moduleName() const { return "ProgrammablePriorityEncoder"; }

    ~ProgrammablePriorityEncoder();
};

/////////////////////////////// Testbench ///////////////////////////////
class tst_PPE : public sc_module {
protected:
    unsigned short nPorts;
public:

    // Interface
    sc_in<bool>                  i_CLK;          // Clock

    // Signals
    sc_signal<bool>              w_RST;          // Reset
    sc_vector<sc_signal<bool> >  w_REQUEST;      // Requests
    sc_vector<sc_signal<bool> >  w_PRIORITY;     // Priorities
    sc_vector<sc_signal<bool> >  w_GRANT;        // Grants
    sc_signal<bool>              w_IDLE;         // Status

    // DUT
    ProgrammablePriorityEncoder* u_PPE;

    // Trace file
    sc_trace_file* tf;

    // Module process
    void p_stimulus();

    SC_HAS_PROCESS(tst_PPE);
    tst_PPE(sc_module_name mn, unsigned short nPorts);

    ~tst_PPE();
};

#endif // PROGRAMMABLEPRIORITYENCODER_H
