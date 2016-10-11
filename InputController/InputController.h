/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : InputController
FILE   : InputController.h
--------------------------------------------------------------------------------
DESCRIPTION: Controller responsible to detect the header of an incoming packet,
schedule an output channel to be requested (by running a routing algorithm), and
hold the request until the packet trailer is delivered.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 31/08/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef __INPUTCONTROLLER_H__
#define __INPUTCONTROLLER_H__

#include "../Routing/Routing.h"
#include "../RequestRegister/RequestRegister.h"

/////////////////////////////////////////////////////////////
/// Input Controller
/////////////////////////////////////////////////////////////
/*!
 * \brief The InputController class is the implementation
 * of input controller from SoCIN.
 *
 * Controller responsible to detect the header of an incoming
 * packet, schedule an output channel to be requested
 * (by running a routing algorithm), and hold the request
 * until the packet trailer is delivered.
 */
class InputController : public SoCINModule {
protected:
    unsigned short numPorts;
public:
    // Interface - System signals
    sc_in<bool> i_CLK;      // Clock
    sc_in<bool> i_RST;      // Reset

    // FIFO interface
    sc_in<bool> i_READ;     // Command to read a data from the FIFO
    sc_in<bool> i_READ_OK;  // FIFO has a data to be read (not empty)
    sc_in<Flit> i_DATA;     // FIFO data output

    // Status of the output channels
    sc_vector<sc_in<bool> > i_IDLE;  // Idle signals from the output ports

    // Requests
    sc_vector<sc_out<bool> > o_REQUEST;    // Requests from the input ports
    sc_out<bool>             o_REQUESTING; // There exists someone requesting

    // Internal signals - data variables which implement connectors
    sc_vector<sc_signal<bool> > w_REQUEST; // Request wires between routing and request register

    unsigned short XID, YID, PORT_ID;

    // Internal Units
    RequestRegister* u_REQ_REG;
    IRouting*        u_ROUTING;

    InputController(sc_module_name mn,
             unsigned short nPorts,
             IRouting* routing,
             unsigned short XID,
             unsigned short YID,
             unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::TInputController; }
    const char* moduleName() const { return "InputController"; }

    ~InputController();
};

///////////////////////////////////////////////////////////////////////////////////////////
//// Implementation
///////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief InputController::InputController The input controller constructor that
 * perform the port binding between the internal modules
 * \param mn Module name
 * \param nPorts Number of ports
 * \param routing Routing module to be used
 * \param XID X identifier in the network
 * \param YID Y identifier in the network
 * \param PORT_ID Port identifier
 */
inline InputController::InputController(sc_module_name mn,
                                 unsigned short nPorts,
                                 IRouting *routing,
                                 unsigned short XID,
                                 unsigned short YID,
                                 unsigned short PORT_ID)
    : SoCINModule(mn) , numPorts(nPorts),
      i_CLK("InputController_iCLK"),
      i_RST("InputController_iRST"),
      i_READ("InputController_iREAD"),
      i_READ_OK("InputController_iREAD_OK"),
      i_DATA("InputController_iDATA"),
      i_IDLE("InputController_iIDLE",nPorts),
      o_REQUEST("InputController_oREQUEST",nPorts),
      o_REQUESTING("InputController_oREQUESTING"),
      w_REQUEST("InputController_wREQUEST",nPorts),
      XID(XID), YID(YID), PORT_ID(PORT_ID),
      u_REQ_REG(NULL), u_ROUTING(routing)
{
    // Binding ports
    // Routing
    u_ROUTING->i_READ_OK(i_READ_OK);
    u_ROUTING->i_DATA(i_DATA);
    u_ROUTING->i_IDLE(i_IDLE);
    u_ROUTING->o_REQUEST(w_REQUEST);

    // Request register
    u_REQ_REG = new RequestRegister("ReqReg",nPorts,XID,YID,PORT_ID);
    u_REQ_REG->i_CLK(i_CLK);
    u_REQ_REG->i_RST(i_RST);
    u_REQ_REG->i_DATA(i_DATA);
    u_REQ_REG->i_READ(i_READ);
    u_REQ_REG->i_READ_OK(i_READ_OK);
    u_REQ_REG->i_REQUEST(w_REQUEST);
    u_REQ_REG->o_REQUEST(o_REQUEST);
    u_REQ_REG->o_REQUESTING(o_REQUESTING);
}

/*!
 * \brief InputController::~InputController Input controller destructor
 */
inline InputController::~InputController() {
    delete u_REQ_REG;
}
/////////////////////////////////////////////////////////////
/// END Input Controller
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Input Controller BUS
/////////////////////////////////////////////////////////////
/*!
 * \brief The InputController class is the implementation
 * of input controller from SoCIN to bus implementation.
 *
 * Controller responsible to detect the header of an incoming
 * packet, generate a requisition and extract the destination
 * address in a one hot fashion (by running a routing algorithm),
 * and hold the request until the packet trailer is delivered.
 */
class InputControllerBus : public SoCINModule {
protected:
    unsigned short numPorts;
public:
    // Interface - System signals
    sc_in<bool> i_CLK;      // Clock
    sc_in<bool> i_RST;      // Reset

    // FIFO interface
    sc_in<bool> i_READ;     // Command to read a data from the FIFO
    sc_in<bool> i_READ_OK;  // FIFO has a data to be read (not empty)
    sc_in<Flit> i_DATA;     // FIFO data output

    // Status of the output channels
    sc_in<bool> i_IDLE;  // Arbiter is idle

    // Requests
    sc_out<bool>             o_REQUEST;     // Request to arbiter
    sc_vector<sc_out<bool> > o_DESTINATION; // Destination address in one hot fashion port request

    unsigned short XID, YID, PORT_ID;

    // Module's process
    void p_OUTPUTS();

    SC_HAS_PROCESS(InputControllerBus);
    InputControllerBus(sc_module_name mn,
                       unsigned short nPorts,
                       unsigned short XID,
                       unsigned short YID,
                       unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::TInputController; }
    const char* moduleName() const { return "InputControllerBus"; }

    ~InputControllerBus();
};

///////////////////////////////////////////////////////////////////////////////////////////
//// Implementation
///////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief InputControllerBus::InputControllerBus The bus input controller constructor that
 * register the process.
 * \param mn Module name
 * \param nPorts Number of ports
 * \param XID X identifier in the network
 * \param YID Y identifier in the network
 * \param PORT_ID Port identifier
 */
inline InputControllerBus::InputControllerBus(sc_module_name mn,
                                              unsigned short nPorts,
                                              unsigned short XID,
                                              unsigned short YID,
                                              unsigned short PORT_ID)
    : SoCINModule(mn) , numPorts(nPorts),
      i_CLK("InputControllerBus_iCLK"),
      i_RST("InputControllerBus_iRST"),
      i_READ("InputControllerBus_iREAD"),
      i_READ_OK("InputControllerBus_iREAD_OK"),
      i_DATA("InputControllerBus_iDATA"),
      i_IDLE("InputControllerBus_iIDLE"),
      o_REQUEST("InputControllerBus_oREQUEST"),
      o_DESTINATION("InputControllerBus_oDESTINATION",nPorts),
      XID(XID), YID(YID), PORT_ID(PORT_ID)
{
    SC_METHOD(p_OUTPUTS);
    sensitive << i_CLK.pos() << i_RST;
}

inline void InputControllerBus::p_OUTPUTS() {
    UIntVar v_DATA;     // Used to extract fields from DATA_IN
    bool    v_BOP;      // Packet framing bit: Begin of packet
    bool    v_EOP;      // Packet framing bit: End of packet
    UIntVar v_X_DEST(0,RIB_WIDTH);  // X-coordinate to destination
    UIntVar v_Y_DEST(0,RIB_WIDTH);  // Y-coordinate to destination

    UIntVar v_DESTINATION(0,numPorts); // The translated address from Mesh 2D to bus

    // It copies the data input to extract the framing and circuit switching commands/bits
    Flit f = i_DATA.read();
    v_DATA = f.data;

    // It decodes the framing commands (bits)
    v_BOP = v_DATA[FLIT_WIDTH-2];
    v_EOP = v_DATA[FLIT_WIDTH-1];
    v_X_DEST  = v_DATA.range(RIB_WIDTH-1,RIB_WIDTH/2);
    v_Y_DEST  = v_DATA.range(RIB_WIDTH/2-1,0);

    v_DESTINATION = COORDINATE_TO_ID(v_X_DEST.to_uint(),v_Y_DEST.to_uint());

    // If RST == 1 disable arbiter request
    if( i_RST.read() == 1 ) {
        o_REQUEST.write(0);
        for(unsigned short i = 0; i < numPorts; i++) {
            o_DESTINATION[i].write(0);
        }
    } else {
        // If there is data on buffer to read and BOP and request is low
        // then enable request and get the destination address
        if( (i_READ_OK.read() == 1) && (v_BOP == 1) && (o_REQUEST.read() == 0) ) {
            o_REQUEST.write(1);
            for(unsigned short i = 0; i < numPorts; i++) {
                if( i == v_DESTINATION ) {
                    o_DESTINATION[i].write(1);
                } else {
                    o_DESTINATION[i].write(0);
                }
            }
        } else {
            // If there is data on buffer to read and EOP and read command
            // then disable request
            if( (i_READ_OK.read() == 1) && (v_EOP == 1) && (i_READ.read() == 1) ) {
                o_REQUEST.write(0);
            }
        }
    }

}

/*!
 * \brief InputControllerBus::~InputControllerBus Bus input controller destructor
 */
inline InputControllerBus::~InputControllerBus() {}
/////////////////////////////////////////////////////////////
/// END Input Controller
/////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Testbench for routing
/////////////////////////////////////////////////////////////

class tst_InputController : public sc_module {
private:
    unsigned short nPorts;
public:
    // Interface - System signals
    sc_in<bool>     i_CLK;      // Clock
    sc_signal<bool> w_RST;      // Reset

    // FIFO interface
    sc_signal<bool> w_READ;     // Command to read a data from the FIFO
    sc_signal<bool> w_READ_OK;  // FIFO has a data to be read (not empty)
    sc_signal<Flit> w_DATA;     // FIFO data output

    // Status of the output channels
    sc_vector<sc_signal<bool> > w_IDLE;  // Idle signals from the output ports

    // Requests
    sc_vector<sc_signal<bool> > w_REQUEST;    // Requests from the input ports
    sc_signal<bool>             w_REQUESTING; // There exists someone requesting

    // DUT - Design Under Test
    InputController* u_IC;

    // Trace file
    sc_trace_file* tf;

    void p_STIMULUS();

    unsigned short XID, YID, PORT_ID;

    SC_HAS_PROCESS(tst_InputController);
    tst_InputController(sc_module_name mn,
                        IRouting* routing,
                        unsigned short nPorts,
                        unsigned short XID,
                        unsigned short YID,
                        unsigned short PORT_ID);
    ~tst_InputController();
};


////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
////////////////////////////// Testbench ///////////////////////////////
////////////////////////////////////////////////////////////////////////
inline tst_InputController::tst_InputController(sc_module_name mn,
                                         IRouting *routing,
                                         unsigned short nPorts,
                                         unsigned short XID,
                                         unsigned short YID,
                                         unsigned short PORT_ID)
    : sc_module(mn),
      nPorts(nPorts),
      i_CLK("tst_IC_iCLK"),
      w_RST("tst_IC_wRST"),
      w_READ("tst_IC_wREAD"),
      w_READ_OK("tst_IC_wREAD_OK"),
      w_DATA("tst_IC_wDATA"),
      w_IDLE("tst_IC_wIDLE",nPorts),
      w_REQUEST("tst_IC_wREQUEST",nPorts),
      w_REQUESTING("tst_IC_wREQUESTING"),
      XID(XID),YID(YID),PORT_ID(PORT_ID)
{
    u_IC = new InputController("IC",nPorts,routing,XID,YID,PORT_ID);
    u_IC->i_CLK(i_CLK);
    u_IC->i_RST(w_RST);
    u_IC->i_DATA(w_DATA);
    u_IC->i_IDLE(w_IDLE);
    u_IC->i_READ(w_READ);
    u_IC->i_READ_OK(w_READ_OK);
    u_IC->o_REQUEST(w_REQUEST);
    u_IC->o_REQUESTING(w_REQUESTING);

    tf = sc_create_vcd_trace_file("InputController");
    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,w_RST,"RST");
    sc_trace(tf,w_DATA,"DATA");
    sc_trace(tf,w_READ,"RD");
    sc_trace(tf,w_READ_OK,"ROK");
    sc_trace(tf,w_REQUESTING,"REQUESTING");
    for(unsigned short i = 0; i < nPorts; i++) {
        char strIdle[15];
        sprintf(strIdle,"IDLE(%u)",i);
        sc_trace(tf,w_IDLE[i],strIdle);

        char strReq[20];
        sprintf(strReq,"REQUEST(%u)",i);
        sc_trace(tf,w_REQUEST[i],strReq);
    }

    // Register stimulus module
    SC_CTHREAD(p_STIMULUS,i_CLK.pos());
    sensitive << i_CLK;
}

inline tst_InputController::~tst_InputController() {
    sc_close_vcd_trace_file(tf);
    delete u_IC;
}

inline void tst_InputController::p_STIMULUS() {

    unsigned short i;

    w_RST.write(true);
    wait();
    w_RST.write(false);
    wait();

    sc_uint<8> v_SRC;
    sc_uint<8> v_DEST;

    UIntVar framing;
    UIntVar flit;

    framing[FLIT_WIDTH-2] = 1; // BOP
    v_SRC = (XID << RIB_WIDTH/2) | YID;
    v_DEST = 0x00;

    // Assemble a header packet with specified src and dst addresses
    flit =  ( (framing) | ( v_SRC << RIB_WIDTH ) | v_DEST );

    // Assemble transfer unit with the data previous assembled
    Flit flitTest(flit,NULL);

    // Write data in the channel
    w_DATA.write(flitTest);

    // Enable idle signals
    for( i = 0; i < nPorts; i++ ) {
        w_IDLE[i].write(true);
    }

    // Enable read ok
    w_READ_OK.write(true);
    wait();

    // Command to read
    w_READ.write(true);
    wait();
    wait();

    // End the simulation
    sc_stop();
}


#endif // INPUTCONTROLLER_H
