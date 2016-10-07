/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : IArbiter
FILE   : Arbiter.h
--------------------------------------------------------------------------------
DESCRIPTION: The Arbiter interface class
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 12/08/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef __ARBITER_H__
#define __ARBITER_H__

#include "../PriorityGenerator/PriorityGenerator.h"
#include "../ProgrammablePriorityEncoder/ProgrammablePriorityEncoder.h"

/////////////////////////////////////////////////////////////
/// Interface for Arbiters
/////////////////////////////////////////////////////////////
/*!
 * \brief The Arbiter class represents the abstract class of
 * Arbiter entity used by OC (Output Controller)
 */
class IArbiter : public SoCINModule {
protected:
    unsigned short nPorts;
public:
    // System interfaces
    sc_in<bool> i_CLK; // Clock
    sc_in<bool> i_RST; // Reset

    // Arbitration interfaces
    sc_vector<sc_in<bool> >  i_REQUEST; // Requests
    sc_vector<sc_out<bool> > o_GRANT;   // Grants
    sc_out<bool>             o_IDLE;    // Status

    // Internal data structures
    unsigned short XID, YID, PORT_ID;

    IArbiter(sc_module_name mn,
             unsigned short nPorts,
             unsigned short XID,
             unsigned short YID,
             unsigned short PORT_ID)
        : SoCINModule(mn),
          nPorts(nPorts),
          i_CLK("Arb_iCLK"),
          i_RST("Arb_iRST"),
          i_REQUEST("Arb_REQ",nPorts),
          o_GRANT("Arb_oGRANT",nPorts),
          o_IDLE("Arb_oIDLE"),
          XID(XID),
          YID(YID),
          PORT_ID(PORT_ID) {}

    ~IArbiter() = 0;
};
/// \brief IArbiter::~IArbiter Virtual
/// destructor of abstract class
inline IArbiter::~IArbiter() {}
/////////////////////////////////////////////////////////////
/// End of interface for Arbiters
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
/// Distributed arbiter (original SoCIN)
/////////////////////////////////////////////////////////////
/*!
 * \brief The DistributedArbiter class represents the original
 * SoCIN arbiter composed by a Priority Generator (plugin-based) and
 * a Programmable Priority Encoder
 */
class DistributedArbiter : public IArbiter {
public:

    // Internal signals
    sc_vector<sc_signal<bool> > w_PRIORITY; // Priorities

    // Internal Units - Components
    IPriorityGenerator* u_PG;
    ProgrammablePriorityEncoder* u_PPE;

    SC_HAS_PROCESS(IArbiter);
    DistributedArbiter(sc_module_name mn,
                       unsigned short nPorts,
                       IPriorityGenerator* pg,
                       unsigned short XID,
                       unsigned short YID,
                       unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::TArbiter; }
    const char* moduleName() const { return "DistributedArbiter"; }
    ~DistributedArbiter();
};
/////////////////////////////////////////////////////////////
/// End of Distributed arbiter
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
/// Arbiter testbench
/////////////////////////////////////////////////////////////
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
    IArbiter* arb;

    // Trace file
    sc_trace_file* tf;

    void p_stimulus();

    SC_HAS_PROCESS(tst_Arbiter);
    tst_Arbiter(sc_module_name mn,IArbiter* arb,unsigned short nPorts);

    ~tst_Arbiter();
};

////////////////////////////////////////////////////////////////////////////////
//// Implementation
////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
/// Distributed arbiter (original SoCIN)
/////////////////////////////////////////////////////////////
inline DistributedArbiter::DistributedArbiter(sc_module_name mn,
                                       unsigned short nPorts,
                                       IPriorityGenerator *pg,
                                       unsigned short XID,
                                       unsigned short YID,
                                       unsigned short PORT_ID)
        : IArbiter(mn,nPorts,XID,YID,PORT_ID),
          w_PRIORITY("DistArb_wPRIOR",nPorts),
          u_PG(pg),
          u_PPE(NULL)
{
    u_PPE = new ProgrammablePriorityEncoder("PPE",nPorts,XID,YID,PORT_ID);
    // Binding ports
    // PPE
    u_PPE->i_CLK(i_CLK);
    u_PPE->i_RST(i_RST);
    u_PPE->i_REQUEST(i_REQUEST);
    u_PPE->i_PRIORITY(w_PRIORITY);
    u_PPE->o_GRANT(o_GRANT);
    u_PPE->o_IDLE(o_IDLE);

    // PG
    u_PG->i_CLK(i_CLK);
    u_PG->i_RST(i_RST);
    u_PG->i_GRANTS(o_GRANT);
    u_PG->o_PRIORITIES(w_PRIORITY);

}

inline DistributedArbiter::~DistributedArbiter() {
    delete u_PPE;
}
/////////////////////////////////////////////////////////////
/// End of distributed arbiter
/////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////
/// Arbiter testbench
/////////////////////////////////////////////////////////////
inline tst_Arbiter::tst_Arbiter(sc_module_name mn, IArbiter *arb, unsigned short nPorts)
        : sc_module(mn), nPorts(nPorts), i_CLK("tst_Arb_iCLK"), w_RST("tst_Arb_wRST"),
          w_REQUEST("tst_Arb_wREQ",nPorts), w_GRANT("tst_Arb_wGRANT",nPorts),
          w_IDLE("tst_Arb_wIDLE"),arb(arb) {

    // Bind
    arb->i_CLK(i_CLK);
    arb->i_RST(w_RST);
    arb->i_REQUEST(w_REQUEST);
    arb->o_GRANT(w_GRANT);
    arb->o_IDLE(w_IDLE);

    // Trace
    tf = sc_create_vcd_trace_file("arbiter_waves");
    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,w_RST,"RST");
    sc_trace(tf,w_IDLE,"IDLE");
    for(unsigned short i = 0; i < nPorts; i++) {
        char strReq[10];
        sprintf(strReq,"REQ(%u)",i);
        sc_trace(tf,w_REQUEST[i],strReq);

        char strGnt[15];
        sprintf(strGnt,"GRANT(%u)",i);
        sc_trace(tf,w_GRANT[i],strGnt);

    }

    // Defining testbench stimulus process
    SC_THREAD(p_stimulus);
//    sensitive << i_CLK;

}

inline tst_Arbiter::~tst_Arbiter() {
    sc_close_vcd_trace_file(tf);
    delete arb;
}

inline void tst_Arbiter::p_stimulus() {

    unsigned short i;

    // Initialize reseted
    w_RST.write(true);
    wait(100,SC_NS);

    w_RST.write(false);

    for( unsigned short x = 0; x <= nPorts; x++ ) {
        // All request OFF
        for(i = 0; i < nPorts; i++) {
            w_REQUEST[i].write(false);
        }
        wait(400,SC_NS);
//        wait();
//        wait();
//        wait();

        // All request ON
        for(i = 0; i < nPorts; i++) {
            w_REQUEST[i].write(true);
        }
        wait(400,SC_NS);
//        wait();
//        wait();
//        wait();
    }

    // Disabling one request per cycle
    for( i = 0; i < nPorts; i++ ) {
        w_REQUEST[i].write(false);
        wait(400,SC_NS);
//        wait();
//        wait();
//        wait();
    }

    // Specific for 4 bit
    if( nPorts == 4 ) {
        // 0b0001
        w_REQUEST[0].write(true);
        wait(400,SC_NS);
//        wait();
//        wait();
//        wait();

        // 0b0000
        w_REQUEST[0].write(false);
        wait(400,SC_NS);
//        wait();
//        wait();
//        wait();

        // 0b1001
        w_REQUEST[0].write(true);
        w_REQUEST[3].write(true);
        wait(400,SC_NS);
//        wait();
//        wait();
//        wait();

    }

    for( unsigned short x = 0; x < 1; x++ ) {
        // All request OFF
        for(i = 0; i < nPorts; i++) {
            w_REQUEST[i].write(false);
        }
        wait(400,SC_NS);
//        wait();
//        wait();
//        wait();

        // All request ON
        for(i = 0; i < nPorts; i++) {
            w_REQUEST[i].write(true);
        }
        wait(400,SC_NS);
//        wait();
//        wait();
//        wait();
    }

    // All request OFF
    for(i = 0; i < nPorts; i++) {
        w_REQUEST[i].write(false);
    }
//    wait();
//    wait();
//    wait();
    wait(400,SC_NS);

    sc_stop();
}
/////////////////////////////////////////////////////////////
/// End of arbiter testbench
/////////////////////////////////////////////////////////////


#endif // ARBITER_H
