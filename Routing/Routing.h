/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : IRouting - IMesh2DRouting
FILE   : IRouting.h
--------------------------------------------------------------------------------
DESCRIPTION: It is the interface classes for implement routing algorithms
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
#ifndef __ROUTING_H__
#define __ROUTING_H__

#include "../SoCINModule.h"
#include "../SoCINDefines.h"

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
    // FIFO interface
    sc_in<bool> i_READ_OK;  // FIFO has a data to be read (not empty)
    sc_in<Flit> i_DATA;     // FIFO data output

    // Status of the output channels
    sc_vector<sc_in<bool> > i_IDLE;  // Idle signals from the output ports

    // Requests
    sc_vector<sc_out<bool> > o_REQUEST;    // Requests from the input ports

    unsigned short XID, YID;

    IRouting(sc_module_name mn,
             unsigned short nPorts,
             unsigned short XID,
             unsigned short YID)
        : SoCINModule(mn) , numPorts(nPorts),
          i_READ_OK("IRouting_iREAD_OK"),
          i_DATA("IRouting_iDATA"),
          i_IDLE("IRouting_iIDLE",nPorts),
          o_REQUEST("IRouting_oREQUEST",nPorts),
          XID(XID), YID(YID) {}

    ModuleType moduleType() const { return SoCINModule::TRouting; }

    ~IRouting() = 0;
};
inline IRouting::~IRouting() {}
/////////////////////////////////////////////////////////////
/// END Interface for Routing
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Interface for Mesh 2D topology routing
/////////////////////////////////////////////////////////////
/*!
 * \brief The IMesh2DRouting class is an interface
 * (abstract class) for Mesh 2D routing implementations
 */
class IMesh2DRouting : public IRouting {
protected:
    // Request encoding
    unsigned char REQ_L;
    unsigned char REQ_N;
    unsigned char REQ_E;
    unsigned char REQ_S;
    unsigned char REQ_W;
    unsigned char REQ_NONE;
public:

    IMesh2DRouting(sc_module_name mn,
             unsigned short nPorts,
             unsigned short XID,
             unsigned short YID)
        : IRouting(mn,nPorts,XID,YID),
          REQ_L(1),     // Local port is always the first requisition - 0b00001
          REQ_N(0),     // Initialize ALL communication requests on 0
          REQ_E(0),     // Initialize ALL communication requests on 0
          REQ_S(0),     // Initialize ALL communication requests on 0
          REQ_W(0),     // Initialize ALL communication requests on 0
          REQ_NONE(0)   // None request - always 0
    {

        //  TODO: Melhorar - useNorth, useEast ...
        if( XID == 0 ) { // First column
            if( YID == 0 ) {// First line (Corner Left-Bottom) (X == 0 && Y == 0) -> REQ0 = LOCAL | REQ1 = NORTH | REQ2 = EAST
                REQ_N = 2;  // NORTH: 0b00010
                REQ_E = 4;  // EAST : 0b00100
            } else if( YID == Y_SIZE-1) { // Last line (Corner Left-Up) (X == 0 && Y == ySize-1) -> R0=L | R1=E | R2=S
                REQ_E = 2;  // EAST : 0b00010
                REQ_S = 4;  // SOUTH: 0b00100
            } else {            // Middle lines (Left border) (X == 0 &&  0 < Y < ySize-1) -> R0=L | R1=N | R2=E | R3=S
                REQ_N = 2;  // NORTH: 0b00010
                REQ_E = 4;  // EAST : 0b00100
                REQ_S = 8;  // SOUTH: 0b01000
            }
        } else if( XID == X_SIZE-1 ) { // Last column
            if( YID == 0 ) {    // First line (Corner Right-Bottom) (X == xSize-1 && Y == 0) -> R0 = L | R1 = N | R2 = W
                REQ_N = 2;  // NORTH: 0b00010
                REQ_W = 4;  // WEST : 0b00100
            } else if( YID == Y_SIZE-1) { // Last line (Corner Right-Up) (X == xSize-1 && Y == ySize-1) -> R0=L | R1=S | R2=W
                REQ_S = 2;  // EAST : 0b00010
                REQ_W = 4;  // SOUTH: 0b00100
            } else { // Middle lines (Right border) (X == xSize-1 &&  0 < Y < ySize-1) -> R0=L | R1=N | R2=S | R3=W
                REQ_N = 2;  // NORTH: 0b00010
                REQ_S = 4;  // SOUTH: 0b00100
                REQ_W = 8;  // WEST : 0b01000
            }
        } else { // Middle columns
            if( YID == 0 ) {// First line (Bottom border) ( 0 < X < xSize-1 && Y == 0) -> REQ0 = LOCAL | REQ1 = NORTH | REQ2 = EAST | REQ3 = WEST
                REQ_N = 2;  // NORTH: 0b00010
                REQ_E = 4;  // EAST : 0b00100
                REQ_W = 8;  // WEST : 0b01000
            } else if( YID == Y_SIZE-1) { // Last line (Up border) ( 0 < X < xSize && Y == ySize-1) -> R0=L | R1=E | R2=S | R3=W
                REQ_E = 2;  // EAST : 0b00010
                REQ_S = 4;  // SOUTH: 0b00100
                REQ_W = 8;  // WEST : 0b01000
            } else { // Middle lines (Center) ( 0 < X < xSize &&  0 < Y < ySize-1) -> R0=L | R1=N | R2=E | R3=S | R4=W
                REQ_N = 2;  // NORTH: 0b00010
                REQ_E = 4;  // EAST : 0b00100
                REQ_S = 8;  // SOUTH: 0b01000
                REQ_W = 16; // WEST : 0b10000
            }
        }
    }

    ~IMesh2DRouting() = 0;
};
inline IMesh2DRouting::~IMesh2DRouting() {}
/////////////////////////////////////////////////////////////
/// END Interface for Mesh topology routing
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
 * \return A method for instantiate a Routing
 */
typedef IRouting* create_Routing(sc_simcontext*,
                                 sc_module_name,
                                 unsigned short nPorts,
                                 unsigned short int XID,
                                 unsigned short int YID);

/*!
 * \brief destroy_Routing Typedef for deallocating a
 * routing
 */
typedef void destroy_Routing(IRouting*);
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Testbench for routing
/////////////////////////////////////////////////////////////
// TODO: to implement

#endif // __ROUTING_H__
