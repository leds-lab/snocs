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
#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include "../Routing/Routing.h"
#include "../RequestRegister/RequestRegister.h"

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Input Controller
/////////////////////////////////////////////////////////////
/*!
 * \brief The InputController class is the implementation
 * of input controller of from SoCIN.
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
             RequestRegister* reqReg,
             IRouting* routing,
             unsigned short XID,
             unsigned short YID,
             unsigned short PORT_ID);

    ModuleType moduleType() const { return SoCINModule::InputController; }
    const char* moduleName() const { return "InputController"; }

    ~InputController();
};
/////////////////////////////////////////////////////////////
/// END Input Controller
/////////////////////////////////////////////////////////////

#endif // INPUTCONTROLLER_H
