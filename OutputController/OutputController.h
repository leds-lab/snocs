/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : OutputController
FILE   : OutputController.h
--------------------------------------------------------------------------------
DESCRIPTION: Controller responsible to schedule the use of the associated
output channel. It is based on an arbiter that receives requests and, based
on an arbitration algorithm, selects one request to be granted. A grant is
held at the high level while the request equals 1.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 31/08/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef OUTPUTCONTROLLER_H
#define OUTPUTCONTROLLER_H

#include "../Arbiter/Arbiter.h"

/////////////////////////////////////////////////////////////
/// Output Controller
/////////////////////////////////////////////////////////////
/*!
 * \brief The OutputController class is the implementation
 * of Output controller from SoCIN.
 *
 * Controller responsible to schedule the use of the
 * associated output channel.
 */
class OutputController : public SoCINModule {
protected:
    unsigned short numPorts;
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

    // Internal Unit - Sub-module
    IArbiter* u_ARBITER;

    OutputController(sc_module_name mn,
                     unsigned short nPorts,
                     IArbiter* arb,
                     unsigned short XID,
                     unsigned short YID,
                     unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::OC; }
    const char* moduleName() const { return "OutputController"; }

    ~OutputController();
};
/////////////////////////////////////////////////////////////
/// END Output Controller
/////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
//// Implementation
///////////////////////////////////////////////////////////////////////////////////////////
inline OutputController::OutputController(sc_module_name mn,
                                          unsigned short nPorts,
                                          IArbiter *arb,
                                          unsigned short XID,
                                          unsigned short YID,
                                          unsigned short PORT_ID)
    : SoCINModule(mn), numPorts(nPorts),
      i_CLK("OutputController_iCLK"),
      i_RST("OutputController_iRST"),
      i_REQUEST("OutputController_iREQUEST",nPorts),
      o_GRANT("OutputController_oGRANT",nPorts),
      o_IDLE("OutputController_oIDLE"),
      XID(XID),YID(YID),PORT_ID(PORT_ID),
      u_ARBITER(arb)
{
    // Binding ports
    u_ARBITER->i_CLK(i_CLK);
    u_ARBITER->i_RST(i_RST);
    u_ARBITER->i_REQUEST(i_REQUEST);
    u_ARBITER->o_GRANT(o_GRANT);
    u_ARBITER->o_IDLE(o_IDLE);
}


#endif // OUTPUTCONTROLLER_H
