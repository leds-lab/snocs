#ifndef ARBITER_H
#define ARBITER_H

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
                       ProgrammablePriorityEncoder* ppe,
                       unsigned short XID,
                       unsigned short YID,
                       unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::Arbiter; }
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

#endif // ARBITER_H
