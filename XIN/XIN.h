/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : XIN
FILE   : XIN.h
--------------------------------------------------------------------------------
DESCRIPTION: Input channel module for the generation of the input circuitry for
each router port
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 08/09/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef __XIN_H__
#define __XIN_H__

#include "../Memory/Memory.h"
#include "../InputController/InputController.h"
#include "../AndParam/And.h"
#include "../FlowControl/FlowControl.h"
#include "../Multiplexers/Multiplexers.h"
#include "../AndParam/And.h"

/////////////////////////////////////////////////////////////
/// Router input module
/////////////////////////////////////////////////////////////
/*!
 * \brief The XIN class contains the sub-modules of the input
 * channel in the router.
 */
class XIN : public SoCINModule {
protected:
    unsigned short numPorts;
public:
    // System signals
    sc_in<bool>     i_CLK;  // Clock
    sc_in<bool>     i_RST;  // Reset

    // Link signals
    sc_in<Flit>  i_DATA;    // Input data channel
    sc_in<bool>  i_VALID;   // Data validation
    sc_out<bool> o_RETURN;  // Return (ack or credit)

    // Commands and status signals interconnecting input and output channels
    sc_vector<sc_out<bool> > o_X_REQUEST; // Requests to ...
    sc_out<bool>             o_X_READ_OK; // Read ok to the outputs
    sc_vector<sc_in<bool> >  i_X_READ;    // Read commant from output ...
    sc_vector<sc_in<bool> >  i_X_GRANT;   // Grant from the output channel ...
    sc_vector<sc_in<bool> >  i_X_IDLE;    // Status from ...

    // Data to the output channels
    sc_out<Flit>    o_X_DATA;   // Output data bus

    // Internal signals - wires
    sc_signal<bool> w_READ_OK;    // FIFO has a data to be read
    sc_signal<bool> w_WRITE_OK;   // FIFO has room to be written
    sc_signal<bool> w_X_READ;     // Select command to read a data
    sc_signal<bool> w_READ;       // Command to read a data from the FIFO
    sc_signal<bool> w_WRITE;      // Command to write a data into the FIFO
    sc_signal<bool> w_REQUESTING; // There exists someone requesting

    // Internal data structures
    unsigned short int XID, YID, PORT_ID;

    // Internal units - sub-modules
    IMemory*            u_MEMORY;
    InputController*    u_IC;
    And*                u_AND_READ_OK;
    IInputFlowControl*  u_IFC;
    OneHotMux<bool>*    u_IRS;
    And*                u_AND_READ;

    SC_HAS_PROCESS(XIN);
    // Constructor
    /*!
     * \brief XIN Circuitry of input port
     * \param mn XIN module name
     * \param mem Memory to be used by the module
     * \param routing Routing to be used by the module
     * \param ifc Input flow control to be used by the module
     * \param nPorts Number of ports of the router
     * \param XID Network X-coordinate router that contains this module
     * \param YID Network Y-coordinate router that contains this module
     * \param PORT_ID Port identificator inside the router
     */
    XIN(sc_module_name mn,
        IMemory* mem,
        IRouting* routing,
        IInputFlowControl* ifc,
        unsigned short nPorts,
        unsigned short XID,
        unsigned short YID,
        unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::InputModule; }
    const char* moduleName() const { return "XIN"; }

    ~XIN();
};

//////////////////////////////////////////////////////////////////////////////////////
/// Implementation
//////////////////////////////////////////////////////////////////////////////////////
inline XIN::XIN(sc_module_name mn,
                IMemory *mem,
                IRouting *routing,
                IInputFlowControl *ifc,
                unsigned short nPorts,
                unsigned short XID,
                unsigned short YID,
                unsigned short PORT_ID)
    : SoCINModule(mn), numPorts(nPorts),
      i_CLK("XIN_iCLK"),
      i_RST("XIN_iRST"),
      i_DATA("XIN_iDATA"),
      i_VALID("XIN_iVALID"),
      o_RETURN("XIN_oRETURN"),
      o_X_REQUEST("XIN_oX_REQUEST",nPorts),
      o_X_READ_OK("XIN_oX_READ_OK"),
      i_X_READ("XIN_iX_READ",nPorts),
      i_X_GRANT("XIN_iX_GRANT",nPorts),
      i_X_IDLE("XIN_iX_IDLE",nPorts),
      o_X_DATA("XIN_oX_DATA"),
      w_READ_OK("XIN_wREAD_OK"),
      w_WRITE_OK("XIN_wWRITE_OK"),
      w_X_READ("XIN_wX_READ"),
      w_READ("XIN_wREAD"),
      w_WRITE("XIN_wWRITE"),
      w_REQUESTING("XIN_wREQUESTING"),
      XID(XID), YID(YID), PORT_ID(PORT_ID)
{
    // Assign or instantiate sub-modules
    // Assumption: None NULL module is received here
    u_MEMORY      = mem;
    u_IC          = new InputController("IC",nPorts,routing,XID,YID,PORT_ID);
    u_IFC         = ifc;
    u_AND_READ    = new And("AND_READ",2);
    u_AND_READ_OK = new And("AND_READ_OK",2);
    u_IRS         = new OneHotMux<bool>("IRS",nPorts);

    // Port binding
    ////////////////////// Memory //////////////////////
    u_MEMORY->i_CLK(i_CLK);
    u_MEMORY->i_RST(i_RST);
    u_MEMORY->o_READ_OK(w_READ_OK);
    u_MEMORY->o_WRITE_OK(w_WRITE_OK);
    u_MEMORY->i_READ(w_READ);
    u_MEMORY->i_WRITE(w_WRITE);
    u_MEMORY->i_DATA(i_DATA);
    u_MEMORY->o_DATA(o_X_DATA);

    ////////////////////// IC //////////////////////
    u_IC->i_CLK(i_CLK);
    u_IC->i_RST(i_RST);
    u_IC->i_READ_OK(w_READ_OK);
    u_IC->i_READ(w_READ);
    u_IC->i_DATA(o_X_DATA);
    u_IC->i_IDLE(i_X_IDLE);
    u_IC->o_REQUEST(o_X_REQUEST);
    u_IC->o_REQUESTING(w_REQUESTING);

    ////////////////////// AND READ OK //////////////////////
    u_AND_READ_OK->i_DATA[0](w_READ_OK);
    u_AND_READ_OK->i_DATA[1](w_REQUESTING);

    ////////////////////// IFC //////////////////////
    u_IFC->i_CLK(i_CLK);
    u_IFC->i_RST(i_RST);
    u_IFC->i_VALID(i_VALID);
    u_IFC->o_RETURN(o_RETURN);
    u_IFC->i_READ_OK(o_X_READ_OK);
    u_IFC->i_READ(w_READ);
    u_IFC->i_WRITE_OK(w_WRITE_OK);
    u_IFC->o_WRITE(w_WRITE);
    u_IFC->i_DATA(o_X_DATA);

    ////////////////////// IRS //////////////////////
    u_IRS->i_SEL(i_X_GRANT);
    u_IRS->i_DATA(i_X_READ);
    u_IRS->o_DATA(w_X_READ);

    ////////////////////// AND READ //////////////////////
    u_AND_READ->i_DATA[0](w_X_READ);
    u_AND_READ->i_DATA[1](w_REQUESTING);
    u_AND_READ->o_DATA(w_READ);
}

/*!
 * \brief XIN::~XIN Module destructor
 */
inline XIN::~XIN() {
    delete u_IC;
    delete u_AND_READ;
    delete u_AND_READ_OK;
    delete u_IRS;
}

#endif // __XIN_H__
