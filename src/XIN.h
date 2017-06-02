/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : XIN
FILE   : XIN.h
--------------------------------------------------------------------------------
DESCRIPTION: Input channel module for the generation of the input circuitry for
each router port.
XIN_Virtual - a virtual channel of the input module.
XIN_N_VC    - a input module with N virtual channels
XIN_none_VC - the original SoCINfp implementation of the input module w/o VC
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 08/09/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
| 03/10/2016 - 1.1     - Eduardo Alves da Silva      | Dynamic virtual channel
--------------------------------------------------------------------------------
*/
#ifndef __XIN_H__
#define __XIN_H__

#include "../src/Memory.h"
#include "../src/InputController.h"
#include "../src/And.h"
#include "../src/FlowControl.h"
#include "../src/Multiplexers.h"
#include "../src/Demultiplexers.h"
#include "../src/And.h"
#include "../src/PluginManager.h"

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//////////// XIN Virtual - represents a virtual channel in input module ///////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief The XIN_Virtual class represents a virtual channel in the input module
 */
class XIN_Virtual : public SoCINModule {
protected:
    unsigned short numPorts;
public:
    // System signals
    sc_in<bool> i_CLK;  // Clock
    sc_in<bool> i_RST;  // Reset

    // Link signals
    sc_in<Flit> i_DATA; // Input data channel

    // Command and status signals interconnecting input and output channels
    sc_vector<sc_out<bool> > o_X_REQUEST; // Request[Port] to output ports
    sc_vector<sc_in<bool> >  i_X_READ;    // Read[Port] commant from output ports
    sc_vector<sc_in<bool> >  i_X_GRANT;   // Grant[Port] from the output channels
    sc_vector<sc_in<bool> >  i_X_IDLE;    // Status[Port] from output channels
    sc_out<bool>             o_X_READ_OK; // Read ok to the output channels

    // Inputs from IFC
    sc_in<bool>  i_WRITE;   // Write status from IFC - Command to write a data into the FIFO
    sc_out<bool> o_READ;    // Read from (irs-RD . ic-requesting) - Command to read a data from the FIFO
    sc_out<bool> o_WRITE_OK;// FIFO has room to be written

    // Data to the output channels
    sc_out<Flit> o_X_DATA;  // Output data bus

    // Internal signals
    sc_signal<bool> w_READ_OK;    // FIFO has a data to be read
    sc_signal<bool> w_X_READ;     // Selected command to read a data
    sc_signal<bool> w_REQUESTING; // There exists someone requesting

    // Internal units
    IMemory*         u_MEMORY;
    InputController* u_IC;
    And*             u_AND_READ_OK;
    OneHotMux<bool>* u_IRS;
    And*             u_AND_READ;

    SC_HAS_PROCESS(XIN_Virtual);
    XIN_Virtual(sc_module_name mn,
                unsigned short nPorts,
                unsigned short ROUTER_ID,
                unsigned short PORT_ID);

    ~XIN_Virtual();

    // Internal data structures
    unsigned short ROUTER_ID, PORT_ID;

    ModuleType moduleType() const { return SoCINModule::TInputModule; }
    const char* moduleName() const { return "XIN_Virtual"; }
};

//////////////////////////////////////////////////////////////////////////////////////
/// Implementation
//////////////////////////////////////////////////////////////////////////////////////
inline XIN_Virtual::XIN_Virtual(sc_module_name mn,
                         unsigned short nPorts,
                         unsigned short ROUTER_ID,
                         unsigned short PORT_ID)
    : SoCINModule(mn),numPorts(nPorts),
     i_CLK("XIN_Virtual_iCLK"),
     i_RST("XIN_Virtual_iRST"),
     i_DATA("XIN_Virtual_iDATA"),
     o_X_REQUEST("XIN_Virtual_oX_REQUEST",nPorts),
     i_X_READ("XIN_Virtual_iX_READ",nPorts),
     i_X_GRANT("XIN_Virtual_iX_GRANT",nPorts),
     i_X_IDLE("XIN_Virtual_iX_IDLE",nPorts),
     o_X_READ_OK("XIN_Virtual_oX_READ_OK"),
     i_WRITE("XIN_Virtual_iWRITE"),
     o_READ("XIN_Virtual_oREAD"),
     o_WRITE_OK("XIN_Virtual_oWRITE_OK"),
     o_X_DATA("XIN_Virtual_oX_DATA"),
     w_READ_OK("XIN_Virtual_wREAD_OK"),
     w_X_READ("XIN_Virtual_wX_READ"),
     w_REQUESTING("XIN_Virtual_wREQUESTING"),
     ROUTER_ID(ROUTER_ID), PORT_ID(PORT_ID)
{
    // Instantiating sub-modules
    // Assumption: None NULL module is generated here
    IRouting* routing = PLUGIN_MANAGER->routingInstance("Routing",ROUTER_ID,PORT_ID,nPorts);
    u_MEMORY      = PLUGIN_MANAGER->memoryInstance("XIN_Virtual_Memory",ROUTER_ID,PORT_ID,FIFO_IN_DEPTH);
    u_IC          = new InputController("XIN_Virtual_IC",nPorts,routing,ROUTER_ID,PORT_ID);
    u_AND_READ    = new And("XIN_Virtual_AND_READ",2);
    u_AND_READ_OK = new And("XIN_Virtual_AND_READ_OK",2);
    u_IRS         = new OneHotMux<bool>("XIN_Virtual_IRS",nPorts);

    // Port binding
    ////////////////////// Memory //////////////////////
    u_MEMORY->i_CLK(i_CLK);
    u_MEMORY->i_RST(i_RST);
    u_MEMORY->o_READ_OK(w_READ_OK);
    u_MEMORY->o_WRITE_OK(o_WRITE_OK);
    u_MEMORY->i_READ(o_READ);
    u_MEMORY->i_WRITE(i_WRITE);
    u_MEMORY->i_DATA(i_DATA);
    u_MEMORY->o_DATA(o_X_DATA);

    ////////////////////// IC //////////////////////
    u_IC->i_CLK(i_CLK);
    u_IC->i_RST(i_RST);
    u_IC->i_READ_OK(w_READ_OK);
    u_IC->i_READ(o_READ);
    u_IC->i_DATA(o_X_DATA);
    u_IC->i_IDLE(i_X_IDLE);
    u_IC->o_REQUEST(o_X_REQUEST);
    u_IC->o_REQUESTING(w_REQUESTING);

    ////////////////////// AND READ OK //////////////////////
    u_AND_READ_OK->i_DATA[0](w_READ_OK);
    u_AND_READ_OK->i_DATA[1](w_REQUESTING);
    u_AND_READ_OK->o_DATA(o_X_READ_OK);

    ////////////////////// IRS //////////////////////
    u_IRS->i_SEL(i_X_GRANT);
    u_IRS->i_DATA(i_X_READ);
    u_IRS->o_DATA(w_X_READ);

    ////////////////////// AND READ //////////////////////
    u_AND_READ->i_DATA[0](w_X_READ);
    u_AND_READ->i_DATA[1](w_REQUESTING);
    u_AND_READ->o_DATA(o_READ);
}

/*!
* \brief XIN_Virtual::~XIN_Virtual Module destructor
*/
inline XIN_Virtual::~XIN_Virtual() {
    delete u_IC;
    delete u_AND_READ;
    delete u_AND_READ_OK;
    delete u_IRS;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// XIN with N Virtual channels //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Router input module with N virtual channels
/////////////////////////////////////////////////////////////
/*!
 * \brief The XIN_N_VC class contains the sub-modules of the input
 * channel in the router with N virtual channels.
 */
class XIN_N_VC : public SoCINModule {
protected:
    unsigned short numPorts;            // Number of ports in the router
    unsigned short numVirtualChannels;  // Number of virtual channels in the router
    unsigned short widthVcSel;          // Number of bits of virtual channel selector
public:
    // System signals
    sc_in<bool>     i_CLK;  // Clock
    sc_in<bool>     i_RST;  // Reset

    // Link signals
    sc_in<Flit>             i_DATA;        // Input data channel
    sc_in<bool>             i_VALID;       // Data validation
    sc_out<bool>            o_RETURN;      // Return (ack or credit)
    sc_vector<sc_in<bool> > i_VC_SELECTOR; // Input of Virtual channel selector

    // Commands and status signals interconnecting input and output channels
    sc_vector<sc_vector<sc_out<bool> > > o_X_REQUEST;  // Requests[VirtualChannel][Port] to output port
    sc_vector<sc_out<bool> >             o_X_READ_OK;  // Read ok[VirtualChannel] to the outputs
    sc_vector<sc_vector<sc_in<bool> > >  i_X_READ;     // Read[VirtualChannel][Port] commant from output port
    sc_vector<sc_vector<sc_in<bool> > >  i_X_GRANT;    // Grant[VirtualChannel][Port] from the output port
    sc_vector<sc_vector<sc_in<bool> > >  i_X_IDLE;     // Status[VirtualChannel][Port] from output port

    // Data to the output channels
    sc_vector<sc_out<Flit> > o_X_DATA;   // Output data bus

    // Internal signals - wires
    sc_signal<bool> w_READ_OK;    // FIFO has a data to be read
    sc_signal<bool> w_WRITE_OK;   // FIFO has room to be written
    sc_signal<bool> w_READ;       // Command to read a data from the FIFO
    sc_signal<bool> w_WRITE;      // Command to write a data into the FIFO

    sc_signal<Flit> w_DATA_OUT; // Data to be written on output channel
    sc_vector<sc_signal<bool> > w_VC_WRITE;   // Write status - Virtual channels
    sc_vector<sc_signal<bool> > w_VC_READ;    // Read status  - Virtual channels
    sc_vector<sc_signal<bool> > w_VC_WRITE_OK;// Write Ok     - Virtual channels

    // Internal data structures
    unsigned short int ROUTER_ID, PORT_ID;

    // Internal units - sub-modules
    IInputFlowControl*        u_IFC;
    BinaryDemux<bool>*        u_DEMUX_WRITE;
    BinaryMux<bool>*          u_MUX_READ_OK;
    BinaryMux<bool>*          u_MUX_READ;
    BinaryMux<bool>*          u_MUX_WRITE_OK;
    BinaryMux<Flit>*          u_MUX_DATA;
    std::vector<XIN_Virtual*> u_XIN_VC;


    SC_HAS_PROCESS(XIN_N_VC);
    // Constructor
    /*!
     * \brief XIN_N_VC Circuitry of input port with N virtual channels
     * \param mn XIN module name
     * \param nPorts Number of ports of the router
     * \param nVirtualChannels Number of virtual channels
     * \param ROUTER_ID Network router identifier that contains this module
     * \param PORT_ID Port identificator inside the router
     */
    XIN_N_VC(sc_module_name mn,
        unsigned short nPorts,
        unsigned short nVirtualChannels,
        unsigned short ROUTER_ID,
        unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::TInputModule; }
    const char* moduleName() const { return "XIN_N_VC"; }

    ~XIN_N_VC();
};

//////////////////////////////////////////////////////////////////////////////////////
/// Implementation
//////////////////////////////////////////////////////////////////////////////////////
inline XIN_N_VC::XIN_N_VC(sc_module_name mn,
                          unsigned short nPorts,
                          unsigned short nVirtualChannels,
                          unsigned short ROUTER_ID,
                          unsigned short PORT_ID)
    : SoCINModule(mn), numPorts(nPorts),
      numVirtualChannels(nVirtualChannels),
      i_CLK("XIN_N_VC_iCLK"),
      i_RST("XIN_N_VC_iRST"),
      i_DATA("XIN_N_VC_iDATA"),
      i_VALID("XIN_N_VC_iVALID"),
      o_RETURN("XIN_N_VC_oRETURN"),
      i_VC_SELECTOR("XIN_N_VC_iVC_SELECTOR"),
      o_X_REQUEST("XIN_N_VC_oX_REQUEST",nVirtualChannels),
      o_X_READ_OK("XIN_N_VC_oX_READ_OK",nVirtualChannels),
      i_X_READ("XIN_N_VC_iX_READ",nVirtualChannels),
      i_X_GRANT("XIN_N_VC_iX_GRANT",nVirtualChannels),
      i_X_IDLE("XIN_N_VC_iX_IDLE",nVirtualChannels),
      o_X_DATA("XIN_N_VC_oX_DATA",nVirtualChannels),
      w_READ_OK("XIN_N_VC_wREAD_OK"),
      w_WRITE_OK("XIN_N_VC_wWRITE_OK"),
      w_READ("XIN_N_VC_wREAD"),
      w_WRITE("XIN_N_VC_wWRITE"),
      w_DATA_OUT("XIN_N_VC_wDATA_OUT"),
      w_VC_WRITE("XIN_N_VC_wVC_WRITE",nVirtualChannels),
      w_VC_READ("XIN_N_VC_wVC_READ",nVirtualChannels),
      w_VC_WRITE_OK("XIN_N_VC_wVC_WRITE_OK",nVirtualChannels),
      ROUTER_ID(ROUTER_ID),PORT_ID(PORT_ID),
      u_XIN_VC(nVirtualChannels,NULL)
{
    unsigned short i;

    // Configuring channels according the number of ports and virtual channels
    widthVcSel = (unsigned short) ceil(log2(nVirtualChannels));
    i_VC_SELECTOR.init(widthVcSel);
    for( i = 0; i < nVirtualChannels; i++ ) {
        o_X_REQUEST[i].init(nPorts);
        i_X_READ[i].init(nPorts);
        i_X_GRANT[i].init(nPorts);
        i_X_IDLE[i].init(nPorts);
    }

    // Instantiating modules
    u_IFC = PLUGIN_MANAGER->inputFlowControlInstance("XIN_N_VC_IFC",ROUTER_ID,PORT_ID);
    u_DEMUX_WRITE = new BinaryDemux<bool>("XIN_N_VC_demux_WRITE",nVirtualChannels);
    u_MUX_READ_OK = new BinaryMux<bool>("XIN_N_VC_mux_READ_OK",nVirtualChannels);
    u_MUX_READ = new BinaryMux<bool>("XIN_N_VC_mux_READ",nVirtualChannels);
    u_MUX_WRITE_OK = new BinaryMux<bool>("XIN_N_VC_mux_WRITE_OK",nVirtualChannels);
    u_MUX_DATA = new BinaryMux<Flit>("XIN_N_VC_mux_DATA",nVirtualChannels);
    for(i = 0; i < nVirtualChannels; i++) {
        char strXinVC[17];
        sprintf(strXinVC,"XIN_u_VC(%u)",i);
        u_XIN_VC[i] = new XIN_Virtual(strXinVC,nPorts,ROUTER_ID,PORT_ID);
    }

    //=============== Binding ports ===============//
    // IFC
    u_IFC->i_CLK(i_CLK);
    u_IFC->i_RST(i_RST);
    u_IFC->i_VALID(i_VALID);
    u_IFC->o_RETURN(o_RETURN);
    u_IFC->i_READ_OK(w_READ_OK);
    u_IFC->i_READ(w_READ);
    u_IFC->i_WRITE_OK(w_WRITE_OK);
    u_IFC->o_WRITE(w_WRITE);
    u_IFC->i_DATA(w_DATA_OUT);

    // Demux Write
    u_DEMUX_WRITE->i_SEL(i_VC_SELECTOR);
    u_DEMUX_WRITE->i_DATA(w_WRITE);
    u_DEMUX_WRITE->o_DATA(w_VC_WRITE);

    // Mux Read Ok
    u_MUX_READ_OK->i_SEL(i_VC_SELECTOR);
    u_MUX_READ_OK->i_DATA(o_X_READ_OK);
    u_MUX_READ_OK->o_DATA(w_READ_OK);

    // Mux Read
    u_MUX_READ->i_SEL(i_VC_SELECTOR);
    u_MUX_READ->i_DATA(w_VC_READ);
    u_MUX_READ->o_DATA(w_READ);

    // Mux Write Ok
    u_MUX_WRITE_OK->i_SEL(i_VC_SELECTOR);
    u_MUX_WRITE_OK->i_DATA(w_VC_WRITE_OK);
    u_MUX_WRITE_OK->o_DATA(w_WRITE_OK);

    // Mux Data
    u_MUX_DATA->i_SEL(i_VC_SELECTOR);
    u_MUX_DATA->i_DATA(o_X_DATA);
    u_MUX_DATA->o_DATA(w_DATA_OUT);

    // Binding Virtual XIN
    for(i = 0; i < nVirtualChannels; i++) {
        XIN_Virtual* u_XIN = u_XIN_VC[i];
        u_XIN->i_CLK(i_CLK);
        u_XIN->i_RST(i_RST);
        u_XIN->i_DATA(i_DATA);
        u_XIN->o_X_REQUEST(o_X_REQUEST[i]);
        u_XIN->o_X_READ_OK(o_X_READ_OK[i]);
        u_XIN->i_X_READ(i_X_READ[i]);
        u_XIN->i_X_GRANT(i_X_GRANT[i]);
        u_XIN->i_X_IDLE(i_X_IDLE[i]);
        u_XIN->i_WRITE(w_VC_WRITE[i]);
        u_XIN->o_READ(w_VC_READ[i]);
        u_XIN->o_WRITE_OK(w_VC_WRITE_OK[i]);
        u_XIN->o_X_DATA(o_X_DATA[i]);
    }

}

inline XIN_N_VC::~XIN_N_VC() {
    delete u_DEMUX_WRITE;
    delete u_MUX_DATA;
    delete u_MUX_READ;
    delete u_MUX_READ_OK;
    delete u_MUX_WRITE_OK;
    for( unsigned short i = 0; i < numVirtualChannels; i++ ) {
        delete u_XIN_VC[i];
    }
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// XIN none Virtual channels ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Router input module without virtual channels
/////////////////////////////////////////////////////////////
/*!
 * \brief The XIN_none_VC class contains the sub-modules of the input
 * channel in the router without virtual channels.
 */
class XIN_none_VC : public SoCINModule {
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
    unsigned short int ROUTER_ID, PORT_ID;

    // Internal units - sub-modules
    IMemory*            u_MEMORY;
    InputController*    u_IC;
    And*                u_AND_READ_OK;
    IInputFlowControl*  u_IFC;
    OneHotMux<bool>*    u_IRS;
    And*                u_AND_READ;

    SC_HAS_PROCESS(XIN_none_VC);
    // Constructor
    /*!
     * \brief XIN Circuitry of input port
     * \param mn XIN module name
     * \param mem Memory to be used by the module
     * \param routing Routing to be used by the module
     * \param ifc Input flow control to be used by the module
     * \param nPorts Number of ports of the router
     * \param ROUTER_ID Network router identifier that contains this module
     * \param PORT_ID Port identificator inside the router
     */
    XIN_none_VC(sc_module_name mn,
        IMemory* mem,
        IRouting* routing,
        IInputFlowControl* ifc,
        unsigned short nPorts,
        unsigned short ROUTER_ID,
        unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::TInputModule; }
    const char* moduleName() const { return "XIN"; }

    ~XIN_none_VC();
};

//////////////////////////////////////////////////////////////////////////////////////
/// Implementation
//////////////////////////////////////////////////////////////////////////////////////
inline XIN_none_VC::XIN_none_VC(sc_module_name mn,
                IMemory *mem,
                IRouting *routing,
                IInputFlowControl *ifc,
                unsigned short nPorts,
                unsigned short ROUTER_ID,
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
      ROUTER_ID(ROUTER_ID), PORT_ID(PORT_ID)
{
    // Assign or instantiate sub-modules
    // Assumption: None NULL module is received here
    u_MEMORY      = mem;
    u_IC          = new InputController("IC",nPorts,routing,ROUTER_ID,PORT_ID);
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
    u_AND_READ_OK->o_DATA(o_X_READ_OK);

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
inline XIN_none_VC::~XIN_none_VC() {
    delete u_IC;
    delete u_AND_READ;
    delete u_AND_READ_OK;
    delete u_IRS;
}

#endif // __XIN_H__
