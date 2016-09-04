/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : IRouting
FILE   : IRouting.h
--------------------------------------------------------------------------------
DESCRIPTION: It is the interface classes for implement routing algorithm
which determines output port in the router to determine the path to packets
traverse the network
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 31/08/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef ROUTING_H
#define ROUTING_H

#include "../SoCINGlobal.h"

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Interface for Routing
/////////////////////////////////////////////////////////////
/*!
 * \brief The IRouting class is an interface
 * (abstract class) for routing implementations
 */
class IRouting : public SoCINModule {
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

    unsigned short XID, YID, PORT_ID;

    IRouting(sc_module_name mn,
             unsigned short nPorts,
             unsigned short XID,
             unsigned short YID,
             unsigned short PORT_ID)
        : SoCINModule(mn) , numPorts(nPorts),
          i_CLK("IRouting_iCLK"),
          i_RST("IRouting_iRST"),
          i_READ("IRouting_iREAD"),
          i_READ_OK("IRouting_iREAD_OK"),
          i_DATA("IRouting_iDATA"),
          i_IDLE("IRouting_iIDLE",nPorts),
          o_REQUEST("IRouting_oREQUEST",nPorts),
          o_REQUESTING("IRouting_oREQUESTING"),
          XID(XID), YID(YID), PORT_ID(PORT_ID) {}

    ModuleType moduleType() const { return SoCINModule::Routing; }

    ~IRouting() = 0;
};
inline IRouting::~IRouting() {}
/////////////////////////////////////////////////////////////
/// END Interface for Routing
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Typedefs for Factory Methods of concrete Routing
/////////////////////////////////////////////////////////////
/*!
 * \brief create_Routing Typedef for instantiate a routing
 * \param sc_simcontext A pointer of simulation context (required for correct plugins use)
 * \param sc_module_name Name for the module to be instantiated
 * \param nPorts Number of ports
 * \param XID Column identifier of the router in the network
 * \param YID Row identifier of the router in the network
 * \param PORT_ID Port identifier of the router
 * \return A method for instantiate a Routing
 */
typedef IRouting* create_Routing(sc_simcontext*,
                                 sc_module_name,
                                 unsigned short nPorts,
                                 unsigned short int XID,
                                 unsigned short int YID,
                                 unsigned short int PORT_ID);

/*!
 * \brief destroy_Routing Typedef for deallocating a
 * routing
 */
typedef void destroy_Routing(IRouting*);
/////////////////////////////////////////////////////////////

#endif // ROUTING_H
