/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : XOUT
FILE   : XOUT.h
--------------------------------------------------------------------------------
DESCRIPTION: Output channel module for the generation of the output circuitry
for each router port.
XOUT_Virtual - a virtual channel of the output module.
XOUT_N_VC    - a output module with N virtual channels
XOUT_none_VC - the original SoCINfp implementation of the output module w/o VC
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
#ifndef __XOUT_H__
#define __XOUT_H__

#include "../Memory/Memory.h"
#include "../OutputController/OutputController.h"
#include "../FlowControl/FlowControl.h"
#include "../Multiplexers/Multiplexers.h"
#include "../Demultiplexers/Demultiplexers.h"
#include "../PriorityEncoder/PriorityEncoder.h"

#include "../PluginManager/PluginManager.h"

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////// XOUT Virtual - represents a virtual channel in output module //////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief The XOUT_Virtual class represents a virtual channel in the output module
 */
class XOUT_Virtual : public SoCINModule {
protected:
    unsigned short numPorts;
public:
    // System signals
    sc_in<bool> i_CLK;  // Clock
    sc_in<bool> i_RST;  // Reset

    // Link signals
    sc_out<Flit> o_DATA;        // Output data channel
    sc_in<bool>  i_HOLD_SEND;   // Used to pause the sending of flits

    // Commands and status signals to interconnecting input and output channels
    sc_vector<sc_in<bool> >  i_X_REQUEST;   // Requests from input channels ...
    sc_vector<sc_in<bool> >  i_X_READ_OK;   // Read oks from input channels ...
    sc_vector<sc_out<bool> > o_X_GRANT;     // Grants to input channels ...
    sc_out<bool>             o_X_IDLE;      // Status to the input channels
    sc_out<bool>             o_X_READ;      // Read command to the input channels
    sc_vector<sc_in<Flit> >  i_X_DATA;      // Data from input channels ...
    sc_in<bool>              i_READ;        // Read from OFC
    sc_out<bool>             o_READ_OK;     // Read Ok from (!hold_send . rok)

    // Internal signals - wires
    sc_signal<Flit> w_DATA_IN;  // FIFO data input
    sc_signal<bool> w_READ_OK;  // FIFO has a data to be read
    sc_signal<bool> w_WRITE_OK; // FIFO has room to be written
    sc_signal<bool> w_WRITE;    // Command to write a data into the FIFO

    // Internal data structures
    unsigned short XID, YID, PORT_ID;

    // Internal Units - sub-modules
    IMemory*            u_MEMORY;
    OutputController*   u_OC;
    OneHotMux<bool>*    u_OWS;
    OneHotMux<Flit>*    u_ODS;

    // Module's process
    void p_HOLD_SEND() {
        o_READ_OK.write( w_READ_OK.read() and (not i_HOLD_SEND.read()) );
    }

    SC_HAS_PROCESS(XOUT_Virtual);
    /*!
     * \brief XOUT_Virtual Circuitry of output virtual channel
     * \param mn XOUT_Virtual module name
     * \param nPorts Number of ports of the router
     * \param XID Network X-coordinate router that contains this module
     * \param YID Network Y-coordinate router that contains this module
     * \param PORT_ID Port identificator inside the router
     */
    XOUT_Virtual(sc_module_name mn,
         unsigned short nPorts,
         unsigned short XID,
         unsigned short YID,
         unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::OutputModule; }
    const char* moduleName() const { return "XOUT_Virtual"; }

    ~XOUT_Virtual();
};

//////////////////////////////////////////////////////////////////////////////////////
/// Implementation
//////////////////////////////////////////////////////////////////////////////////////
inline XOUT_Virtual::XOUT_Virtual(sc_module_name mn,
                  unsigned short nPorts,
                  unsigned short XID,
                  unsigned short YID,
                  unsigned short PORT_ID)
    : SoCINModule(mn),numPorts(nPorts),
      i_CLK("XOUT_iCLK"),
      i_RST("XOUT_iRST"),
      o_DATA("XOUT_oDATA"),
      i_HOLD_SEND("XOUT_iHOLD_SEND"),
      i_X_REQUEST("XOUT_iX_REQUEST",nPorts),
      i_X_READ_OK("XOUT_iX_READ_OK",nPorts),
      o_X_GRANT("XOUT_oX_GRANT",nPorts),
      o_X_IDLE("XOUT_oX_IDLE"),
      o_X_READ("XOUT_oX_READ"),
      i_X_DATA("XOUT_iX_DATA",nPorts),
      i_READ("XOUT_iREAD"),
      o_READ_OK("XOUT_oREAD_OK"),
      w_DATA_IN("XOUT_wDATA_IN"),
      w_READ_OK("XOUT_wREAD_OK"),
      w_WRITE_OK("XOUT_wWRITE_OK"),
      w_WRITE("XOUT_wWRITE"),
      XID(XID),YID(YID),PORT_ID(PORT_ID)
{
    // Assign or instantiate sub-modules
    // Assumption: None NULL module is generated here - from plugin manager
    IPriorityGenerator* pg = PLUGIN_MANAGER->priorityGeneratorInstance("PG",XID,YID,PORT_ID,nPorts);
    IArbiter* arb = new DistributedArbiter("Arbiter",nPorts,pg,XID,YID,PORT_ID);
    u_MEMORY = PLUGIN_MANAGER->memoryInstance("XOUT_Virtual_Memory",XID,YID,PORT_ID,FIFO_OUT_DEPTH);
    u_OC     = new OutputController("OC",nPorts,arb,XID,YID,PORT_ID);
    u_OWS    = new OneHotMux<bool>("OWS",nPorts);
    u_ODS    = new OneHotMux<Flit>("ODS",nPorts);

    // Port binding
    /////////////////// Memory ///////////////////
    u_MEMORY->i_CLK(i_CLK);
    u_MEMORY->i_RST(i_RST);
    u_MEMORY->o_READ_OK(w_READ_OK);
    u_MEMORY->o_WRITE_OK(o_X_READ);
    u_MEMORY->i_READ(i_READ);
    u_MEMORY->i_WRITE(w_WRITE);
    u_MEMORY->i_DATA(w_DATA_IN);
    u_MEMORY->o_DATA(o_DATA);

    /////////////////// OC ///////////////////
    u_OC->i_CLK(i_CLK);
    u_OC->i_RST(i_RST);
    u_OC->i_REQUEST(i_X_REQUEST);
    u_OC->o_GRANT(o_X_GRANT);
    u_OC->o_IDLE(o_X_IDLE);

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

inline XOUT_Virtual::~XOUT_Virtual() {
    delete u_OC;
    delete u_OWS;
    delete u_ODS;
}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// XOUT with N Virtual channels //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Router output module with N virtual channels
/////////////////////////////////////////////////////////////
/*!
 * \brief The XOUT_N_VC class contains the sub-modules of the output
 * channel in the router with N virtual channels.
 */
class XOUT_N_VC : public SoCINModule {
protected:
    unsigned short numPorts;            // Number of ports in the router
    unsigned short numVirtualChannels;  // Number of virtual channels in the router
    unsigned short widthVcSel;          // Number of bits of virtual channel selector
public:
    // System signals
    sc_in<bool> i_CLK;  // Clock
    sc_in<bool> i_RST;  // Reset

    // Link signals
    sc_out<bool>             o_VALID;       // Data validation
    sc_in<bool>              i_RETURN;      // Return (ack or credit)
    sc_out<Flit>             o_DATA;        // Output data channel
    sc_vector<sc_out<bool> > o_VC_SELECTOR; // Output channel priority selector

    sc_vector<sc_in<bool> > i_HOLD_SEND;   // Used to pause the sending of flits

    // Commands and status signals to interconnecting input and output channels
    sc_vector<sc_vector<sc_in<bool> > >  i_X_REQUEST;   // Requests[VirtualChannel][Port] from input channels ...
    sc_vector<sc_vector<sc_in<bool> > >  i_X_READ_OK;   // Read oks[VirtualChannel][Port] from input channels ...
    sc_vector<sc_vector<sc_out<bool> > > o_X_GRANT;     // Grants[VirtualChannel][Port] to input channels ...
    sc_vector<sc_out<bool> >             o_X_IDLE;      // Status[VirtualChannel] to the input channels
    sc_vector<sc_out<bool> >             o_X_READ;      // Read[VirtualChannel] command to the input channels
    sc_vector<sc_vector<sc_in<Flit> > >  i_X_DATA;      // Data[VirtualChannel][Port] from input channels ...

    // Internal signals - wires
    sc_vector<sc_signal<Flit> > w_VC_DATA_OUT;// Data out VC[VirtualChannel]
    sc_vector<sc_signal<bool> > w_VC_READ_OK; // Read ok[VirtualChannel]
    sc_vector<sc_signal<bool> > w_VC_READ;    // Read[VirtualChannel] status

    sc_signal<bool> w_READ_OK;              // Read ok controlled by i_HOLD_SEND
    sc_signal<bool> w_READ;                 // Command to read a data from the FIFO
    sc_vector<sc_signal<bool> > w_PRIORITY; // Output channel priority selector

    // Internal data structures
    unsigned short XID, YID, PORT_ID;

    // Internal Units - sub-modules
    IOutputFlowControl*        u_OFC;
    BinaryMux<bool>*           u_MUX_READ_OK;
    BinaryDemux<bool>*         u_DEMUX_READ;
    BinaryMux<Flit>*           u_MUX_DATA;
    PriorityEncoder*           u_PE;
    std::vector<XOUT_Virtual*> u_XOUT_VC;

    // Module's process
    void p_WRITE_VC() {
        if( o_VALID.read() ) {
            for( unsigned int i = 0; i < widthVcSel; i++ ) {
                o_VC_SELECTOR[i].write( w_PRIORITY[i].read() );
            }
        }
    }

    SC_HAS_PROCESS(XOUT_N_VC);
    /*!
     * \brief XOUT Circuitry of output port
     * \param mn XOUT_N_VC module name
     * \param nPorts Number of ports of the router
     * \param nVirtualChannels Number of virtual channels
     * \param XID Network X-coordinate router that contains this module
     * \param YID Network Y-coordinate router that contains this module
     * \param PORT_ID Port identificator inside the router
     */
    XOUT_N_VC(sc_module_name mn,
         unsigned short nPorts,
         unsigned short nVirtualChannels,
         unsigned short XID,
         unsigned short YID,
         unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::OutputModule; }
    const char* moduleName() const { return "XOUT_N_VC"; }

    ~XOUT_N_VC();
};

//////////////////////////////////////////////////////////////////////////////////////
/// Implementation
//////////////////////////////////////////////////////////////////////////////////////
inline XOUT_N_VC::XOUT_N_VC(sc_module_name mn,
                  unsigned short nPorts,
                  unsigned short nVirtualChannels,
                  unsigned short XID,
                  unsigned short YID,
                  unsigned short PORT_ID)
    : SoCINModule(mn),numPorts(nPorts),
      numVirtualChannels(nVirtualChannels),
      i_CLK("XOUT_N_VC_iCLK"),
      i_RST("XOUT_N_VC_iRST"),
      o_VALID("XOUT_N_VC_oVALID"),
      i_RETURN("XOUT_N_VC_iRETURN"),
      o_DATA("XOUT_N_VC_oDATA"),
      o_VC_SELECTOR("XOUT_N_VC_oVC_SELECTOR"),
      i_HOLD_SEND("XOUT_N_VC_iHOLD_SEND",nVirtualChannels),
      i_X_REQUEST("XOUT_N_VC_iX_REQUEST",nVirtualChannels),
      i_X_READ_OK("XOUT_N_VC_iX_READ_OK",nVirtualChannels),
      o_X_GRANT("XOUT_N_VC_oX_GRANT",nVirtualChannels),
      o_X_IDLE("XOUT_N_VC_oX_IDLE",nVirtualChannels),
      o_X_READ("XOUT_N_VC_oX_READ",nVirtualChannels),
      i_X_DATA("XOUT_N_VC_iX_DATA",nVirtualChannels),
      w_VC_DATA_OUT("XOUT_N_VC_wVC_DATA_OUT",nVirtualChannels),
      w_VC_READ_OK("XOUT_N_VC_wVC_READ_OK",nVirtualChannels),
      w_VC_READ("XOUT_N_VC_wVC_READ",nVirtualChannels),
      w_READ_OK("XOUT_N_VC_wREAD_OK"),
      w_READ("XOUT_N_VC_wREAD"),
      w_PRIORITY("XOUT_wPRIORITY"),
      XID(XID),YID(YID),PORT_ID(PORT_ID),
      u_XOUT_VC(nVirtualChannels,NULL)
{
    unsigned short i;
    // Configuring channels according the number of ports and virtual channels
    widthVcSel = (unsigned short) ceil(log2(nVirtualChannels));
    o_VC_SELECTOR.init(widthVcSel);
    w_PRIORITY.init(widthVcSel);
    for( i = 0; i < nVirtualChannels; i++ ) {
        i_X_REQUEST[i].init(nPorts);
        i_X_READ_OK[i].init(nPorts);
        o_X_GRANT[i].init(nPorts);
        i_X_DATA[i].init(nPorts);
    }

    // Instantiating sub-modules
    // Assumption: None NULL module is received here by plugin manager
    u_OFC = PLUGIN_MANAGER->outputFlowControlInstance("XOUT_N_VC_OFC",XID,YID,PORT_ID,FIFO_IN_DEPTH);
    u_MUX_READ_OK = new BinaryMux<bool>("XOUT_N_VC_mux_READ_OK",nVirtualChannels);
    u_DEMUX_READ = new BinaryDemux<bool>("XOUT_N_VC_demux_READ",nVirtualChannels);
    u_MUX_DATA = new BinaryMux<Flit>("XOUT_N_VC_mux_DATA",nVirtualChannels);
    u_PE = new PriorityEncoder("XOUT_N_VC_PRIORITY_ENCODER",nVirtualChannels);
    for( i = 0; i < nVirtualChannels; i++) {
        char strXoutVc[18];
        sprintf(strXoutVc,"XOUT_u_VC(%u)",i);
        u_XOUT_VC[i] = new XOUT_Virtual(strXoutVc,nPorts,XID,YID,PORT_ID);
    }


    //=============== Binding ports ===============//
    // OFC
    u_OFC->i_CLK(i_CLK);
    u_OFC->i_RST(i_RST);
    u_OFC->o_VALID(o_VALID);
    u_OFC->i_RETURN(i_RETURN);
    u_OFC->i_READ_OK(w_READ_OK);
    u_OFC->o_READ(w_READ);

    // Mux read ok
    u_MUX_READ_OK->i_SEL(w_PRIORITY);
    u_MUX_READ_OK->i_DATA(w_VC_READ_OK);
    u_MUX_READ_OK->o_DATA(w_READ_OK);

    // Demux read
    u_DEMUX_READ->i_SEL(w_PRIORITY);
    u_DEMUX_READ->i_DATA(w_READ);
    u_DEMUX_READ->o_DATA(w_VC_READ);

    // Mux data
    u_MUX_DATA->i_SEL(w_PRIORITY);
    u_MUX_DATA->i_DATA(w_VC_DATA_OUT);
    u_MUX_DATA->o_DATA(o_DATA);

    // Priority encoder
    u_PE->i_REQUEST(w_VC_READ_OK);
    u_PE->o_GRANT(w_PRIORITY);

    // Binding virtual XOUT
    for(i = 0; i < nVirtualChannels; i++) {
        XOUT_Virtual* u_XOUT = u_XOUT_VC[i];
        u_XOUT->i_CLK(i_CLK);
        u_XOUT->i_RST(i_RST);
        u_XOUT->o_DATA(w_VC_DATA_OUT[i]);
        u_XOUT->i_HOLD_SEND(i_HOLD_SEND[i]);
        u_XOUT->i_X_REQUEST(i_X_REQUEST[i]);
        u_XOUT->i_X_READ_OK(i_X_READ_OK[i]);
        u_XOUT->o_X_GRANT(o_X_GRANT[i]);
        u_XOUT->o_X_IDLE(o_X_IDLE[i]);
        u_XOUT->o_X_READ(o_X_READ[i]);
        u_XOUT->i_X_DATA(i_X_DATA[i]);
        u_XOUT->i_READ(w_VC_READ[i]);
        u_XOUT->o_READ_OK(w_VC_READ_OK[i]);
    }

    // Registering module's process
    SC_METHOD(p_WRITE_VC);
    sensitive << o_VALID;
    for( i = 0; i < widthVcSel; i++) {
        sensitive << w_PRIORITY[i];
    }

}

inline XOUT_N_VC::~XOUT_N_VC() {
    delete u_MUX_READ_OK;
    delete u_DEMUX_READ;
    delete u_MUX_DATA;
    delete u_PE;
    for( unsigned short i = 0; i < numVirtualChannels; i++ ) {
        delete u_XOUT_VC[i];
    }

}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// XOUT none Virtual channels ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Router output module without virtual channels
/////////////////////////////////////////////////////////////
/*!
 * \brief The XOUT_none_VC class contains the sub-modules of the
 * output channel in the router without virtual channel.
 */
class XOUT_none_VC : public SoCINModule {
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

    SC_HAS_PROCESS(XOUT_none_VC);
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
    XOUT_none_VC(sc_module_name mn,
         IMemory* mem,
         IArbiter* arb,
         IOutputFlowControl* ofc,
         unsigned short nPorts,
         unsigned short XID,
         unsigned short YID,
         unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::OutputModule; }
    const char* moduleName() const { return "XOUT"; }

    ~XOUT_none_VC();
};

//////////////////////////////////////////////////////////////////////////////////////
/// Implementation
//////////////////////////////////////////////////////////////////////////////////////
inline XOUT_none_VC::XOUT_none_VC(sc_module_name mn,
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

inline XOUT_none_VC::~XOUT_none_VC() {
    delete u_OC;
    delete u_OWS;
    delete u_ODS;
}

#endif // __XOUT_H__
