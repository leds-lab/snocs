/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : XOUT
FILE   : XOUT.h
--------------------------------------------------------------------------------
DESCRIPTION: Output channel module for the generation of the output circuitry
for each router port.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 08/09/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef __XOUT_H__
#define __XOUT_H__

#include "../Memory/Memory.h"
#include "../OutputController/OutputController.h"
#include "../FlowControl/FlowControl.h"
#include "../Multiplexers/Multiplexers.h"

/////////////////////////////////////////////////////////////
/// Router input module
/////////////////////////////////////////////////////////////
/*!
 * \brief The XOUT class contains the sub-modules of the
 * output channel in the router.
 */
class XOUT : public SoCINModule {
protected:
    unsigned short numPorts;
public:
    // System signals
    sc_in<bool> i_CLK;  // Clock
    sc_in<bool> i_RST;  // Reset

    // Link signals
    sc_out<bool> o_VALID;       // Data validation
    sc_in<bool>  i_RETURN;      // Return (ack or credit)
    sc_in<bool>  i_HOLD_SEND;   // Used to pause the sending of flits
    sc_out<Flit> o_DATA;        // Output data channel

    // Commands and status signals to interconnecting input and output channels
    sc_vector<sc_in<bool> >  i_X_REQUEST;   // Requests from input channels ...
    sc_vector<sc_in<bool> >  i_X_READ_OK;   // Read oks from input channels ...
    sc_vector<sc_out<bool> > o_X_GRANT;     // Grants to input channels ...
    sc_out<bool>             o_X_IDLE;      // Status to the input channels
    sc_out<bool>             o_X_READ;      // Read command to the input channels
    sc_vector<sc_in<Flit> >  i_X_DATA;      // Data from input channels ...

    // Internal signals - wires
    sc_signal<Flit> w_DATA_IN;  // FIFO data input
    sc_signal<bool> w_READ_OK;  // FIFO has a data to be read
    sc_signal<bool> w_READ_OK2; // Read ok controlled by i_HOLD_SEND
    sc_signal<bool> w_WRITE_OK; // FIFO has room to be written
    sc_signal<bool> w_READ;     // Command to read a data from the FIFO
    sc_signal<bool> w_WRITE;    // Command to write a data into the FIFO

    // Internal data structures
    unsigned short XID, YID, PORT_ID;

    // Internal Units - sub-modules
    IMemory*            u_MEMORY;
    OutputController*   u_OC;
    IOutputFlowControl* u_OFC;
    OneHotMux<bool>*    u_OWS;
    OneHotMux<Flit>*    u_ODS;

    // Module's process
    void p_HOLD_SEND() {
        w_READ_OK2.write( w_READ_OK.read() and (not i_HOLD_SEND.read()) );
    }

    SC_HAS_PROCESS(XOUT);
    /*!
     * \brief XOUT Circuitry of output port
     * \param mn XOUT module name
     * \param mem Memory to be used by the module
     * \param arb Arbiter to be used by the module
     * \param ofc Output flow control to be used by the module
     * \param nPorts Number of ports of the router
     * \param XID Network X-coordinate router that contains this module
     * \param YID Network Y-coordinate router that contains this module
     * \param PORT_ID Port identificator inside the router
     */
    XOUT(sc_module_name mn,
         IMemory* mem,
         IArbiter* arb,
         IOutputFlowControl* ofc,
         unsigned short nPorts,
         unsigned short XID,
         unsigned short YID,
         unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::OutputModule; }
    const char* moduleName() const { return "XOUT"; }

    ~XOUT();
};

//////////////////////////////////////////////////////////////////////////////////////
/// Implementation
//////////////////////////////////////////////////////////////////////////////////////
inline XOUT::XOUT(sc_module_name mn,
                  IMemory *mem,
                  IArbiter *arb,
                  IOutputFlowControl *ofc,
                  unsigned short nPorts,
                  unsigned short XID,
                  unsigned short YID,
                  unsigned short PORT_ID)
    : SoCINModule(mn),numPorts(nPorts),
      i_CLK("XOUT_iCLK"),
      i_RST("XOUT_iRST"),
      o_VALID("XOUT_oVALID"),
      i_RETURN("XOUT_iRETURN"),
      i_HOLD_SEND("XOUT_iHOLD_SEND"),
      o_DATA("XOUT_oDATA"),
      i_X_REQUEST("XOUT_iX_REQUEST",nPorts),
      i_X_READ_OK("XOUT_iX_READ_OK",nPorts),
      o_X_GRANT("XOUT_oX_GRANT",nPorts),
      o_X_IDLE("XOUT_oX_IDLE"),
      o_X_READ("XOUT_oX_READ"),
      i_X_DATA("XOUT_iX_DATA",nPorts),
      w_DATA_IN("XOUT_wDATA_IN"),
      w_READ_OK("XOUT_wREAD_OK"),
      w_READ_OK2("XOUT_wREAD_OK2"),
      w_WRITE_OK("XOUT_wWRITE_OK"),
      w_READ("XOUT_wREAD"),
      w_WRITE("XOUT_wWRITE"),
      XID(XID),YID(YID),PORT_ID(PORT_ID)
{
    // Assign or instantiate sub-modules
    // Assumption: None NULL module is received here
    u_MEMORY = mem;
    u_OC     = new OutputController("OC",nPorts,arb,XID,YID,PORT_ID);
    u_OFC    = ofc;
    u_OWS    = new OneHotMux<bool>("OWS",nPorts);
    u_ODS    = new OneHotMux<Flit>("ODS",nPorts);

    // Port binding
    /////////////////// Memory ///////////////////
    u_MEMORY->i_CLK(i_CLK);
    u_MEMORY->i_RST(i_RST);
    u_MEMORY->o_READ_OK(w_READ_OK);
    u_MEMORY->o_WRITE_OK(o_X_READ);
    u_MEMORY->i_READ(w_READ);
    u_MEMORY->i_WRITE(w_WRITE);
    u_MEMORY->i_DATA(w_DATA_IN);
    u_MEMORY->o_DATA(o_DATA);

    /////////////////// OC ///////////////////
    u_OC->i_CLK(i_CLK);
    u_OC->i_RST(i_RST);
    u_OC->i_REQUEST(i_X_REQUEST);
    u_OC->o_GRANT(o_X_GRANT);
    u_OC->o_IDLE(o_X_IDLE);

    /////////////////// OFC ///////////////////
    u_OFC->i_CLK(i_CLK);
    u_OFC->i_RST(i_RST);
    u_OFC->o_VALID(o_VALID);
    u_OFC->i_RETURN(i_RETURN);
    u_OFC->i_READ_OK(w_READ_OK2);
    u_OFC->o_READ(w_READ);

    /////////////////// OWS ///////////////////
    u_OWS->i_SEL(o_X_GRANT);
    u_OWS->i_DATA(i_X_READ_OK);
    u_OWS->o_DATA(w_WRITE);

    /////////////////// ODS ///////////////////
    u_ODS->i_SEL(o_X_GRANT);
    u_ODS->i_DATA(i_X_DATA);
    u_ODS->o_DATA(w_DATA_IN);

    // Registering module's process
    SC_METHOD(p_HOLD_SEND);
    sensitive << w_READ_OK << i_HOLD_SEND;

}

inline XOUT::~XOUT() {
    delete u_OC;
    delete u_OWS;
    delete u_ODS;
}

#endif // __XOUT_H__
