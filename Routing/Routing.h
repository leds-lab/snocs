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

#include "../NoC/NoC.h"

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

    unsigned short ROUTER_ID;

    IRouting(sc_module_name mn,
             unsigned short nPorts,
             unsigned short ROUTER_ID)
        : SoCINModule(mn) , numPorts(nPorts),
          i_READ_OK("IRouting_iREAD_OK"),
          i_DATA("IRouting_iDATA"),
          i_IDLE("IRouting_iIDLE",nPorts),
          o_REQUEST("IRouting_oREQUEST",nPorts),
          ROUTER_ID(ROUTER_ID) {}

    ModuleType moduleType() const { return SoCINModule::TRouting; }
    virtual INoC::TopologyType supportedTopology() const = 0;

    ~IRouting() = 0;
};
inline IRouting::~IRouting() {}
/////////////////////////////////////////////////////////////
/// END Interface for Routing
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Interface for Orthogonal 2D topology routing
/////////////////////////////////////////////////////////////
/*!
 * \brief The IOrthoginal2DRouting class is an interface
 * (abstract class) for Orthogonal 2D routing implementations
 */
class IOrthogonal2DRouting : public IRouting {
protected:
    // Request encoding - char used to minimize the num of bytes used - 1 byte - max. requests one-hot style: 8 (2^8)
    unsigned char REQ_L;
    unsigned char REQ_N;
    unsigned char REQ_E;
    unsigned char REQ_S;
    unsigned char REQ_W;
    unsigned char REQ_NONE;

    unsigned short XID,YID;
public:

    IOrthogonal2DRouting(sc_module_name mn,
             unsigned short nPorts,
             unsigned short ROUTER_ID)
        : IRouting(mn,nPorts,ROUTER_ID),
          REQ_L(1),     // Local port is always the first requisition - 0b00001
          REQ_N(0),     // Initialize ALL communication requests on 0
          REQ_E(0),     // Initialize ALL communication requests on 0
          REQ_S(0),     // Initialize ALL communication requests on 0
          REQ_W(0),     // Initialize ALL communication requests on 0
          REQ_NONE(0)   // None request - always 0
    {
        XID = ID_TO_COORDINATE_2D_X(ROUTER_ID);
        YID = ID_TO_COORDINATE_2D_Y(ROUTER_ID);

        // Regular ParIS router with all ports - LOCAL, NORTH, EAST, SOUTH, WEST
        if( nPorts == 5 ) {
            REQ_N = 2;
            REQ_E = 4;
            REQ_S = 8;
            REQ_W = 16;
        } else {
            // Aux. to identify which port must be used according the network position
            unsigned short useNorth, useEast, useSouth, useWest;

            // Set the use of ports according with the network position
            useNorth = ( YID < Y_SIZE-1 ) ? 1 : 0;   // North port is not used on the top    border
            useEast  = ( XID < X_SIZE-1 ) ? 1 : 0;   // East  port is not used on the right  border
            useSouth = ( YID > 0 ) ? 1 : 0;          // South port is not used on the botton border
            useWest  = ( XID > 0 ) ? 1 : 0;          // West  port is not used on the left   border

            if(useNorth) {
                REQ_N = 2;
            }

            if(useEast) {
                REQ_E = (unsigned char) pow(2,(useNorth+useEast));
            }

            if(useSouth) {
                REQ_S = (unsigned char) pow(2,(useNorth+useEast+useSouth));
            }

            if(useWest) {
                REQ_W = (unsigned char) pow(2,(useNorth+useEast+useSouth+useWest));
            }
        }
    }

    INoC::TopologyType supportedTopology() const { return INoC::TT_Orthogonal2D; }

    ~IOrthogonal2DRouting() = 0;
};
inline IOrthogonal2DRouting::~IOrthogonal2DRouting() {}
/////////////////////////////////////////////////////////////
/// END Interface for Orthogonal 2D topology routing
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Interface for Orthogonal 3D topology routing
/////////////////////////////////////////////////////////////
/*!
 * \brief The IOrthogonal3DRouting class is an interface
 * (abstract class) for Orthogonal 3D routing implementations
 */
class IOrthogonal3DRouting : public IRouting {
protected:
    // Request encoding - char used to minimize the num of bytes used - 1 byte - max. requests one-hot style: 8 (2^8)
    unsigned char REQ_L;    // Local
    unsigned char REQ_N;    // North
    unsigned char REQ_E;    // East
    unsigned char REQ_S;    // South
    unsigned char REQ_W;    // West
    unsigned char REQ_U;    // Up
    unsigned char REQ_D;    // Down
    unsigned char REQ_NONE; // None

    unsigned short XID,YID, ZID;
public:

    IOrthogonal3DRouting(sc_module_name mn,
             unsigned short nPorts,
             unsigned short ROUTER_ID)
        : IRouting(mn,nPorts,ROUTER_ID),
          REQ_L(1),     // Local port is always the first requisition - 0b00001
          REQ_N(0),     // Initialize ALL communication requests on 0
          REQ_E(0),     // Initialize ALL communication requests on 0
          REQ_S(0),     // Initialize ALL communication requests on 0
          REQ_W(0),     // Initialize ALL communication requests on 0
          REQ_U(0),
          REQ_D(0),
          REQ_NONE(0)   // None request - always 0
    {
        XID = ID_TO_COORDINATE_3D_X(ROUTER_ID);
        YID = ID_TO_COORDINATE_3D_Y(ROUTER_ID);
        ZID = ID_TO_COORDINATE_3D_Z(ROUTER_ID);

        // Regular ParIS router to 3D topology with all ports - LOCAL, NORTH, EAST, SOUTH, WEST, UP and DOWN
        if( nPorts == 7 ) {
            REQ_N = 2;
            REQ_E = 4;
            REQ_S = 8;
            REQ_W = 16;
            REQ_U = 32;
            REQ_D = 64;
        } else {
            // Aux. to identify which port must be used according the network position
            unsigned short useNorth, useEast, useSouth, useWest, useUp, useDown;

            // Set the use of ports according with the network position
            useNorth = ( YID < Y_SIZE-1 ) ? 1 : 0;   // North port is not used on the top    border
            useEast  = ( XID < X_SIZE-1 ) ? 1 : 0;   // East  port is not used on the right  border
            useSouth = ( YID > 0 ) ? 1 : 0;          // South port is not used on the botton border
            useWest  = ( XID > 0 ) ? 1 : 0;          // West  port is not used on the left   border
            useUp    = ( ZID < Z_SIZE-1 ) ? 1 : 0;   // Up    port is not used on the upper  layer
            useDown  = ( ZID > 0 ) ? 1 : 0;          // Down  port is not used on the bottom layer

            if(useNorth) {
                REQ_N = 2;
            }

            if(useEast) {
                REQ_E = (unsigned char) pow(2,(useNorth+useEast));
            }

            if(useSouth) {
                REQ_S = (unsigned char) pow(2,(useNorth+useEast+useSouth));
            }

            if(useWest) {
                REQ_W = (unsigned char) pow(2,(useNorth+useEast+useSouth+useWest));
            }

            if(useUp) {
                REQ_U = (unsigned char) pow(2,(useNorth+useEast+useSouth+useWest+useUp));
            }

            if(useDown) {
                REQ_D = (unsigned char) pow(2,(useNorth+useEast+useSouth+useWest+useUp+useDown));
            }
        }
    }

    INoC::TopologyType supportedTopology() const { return INoC::TT_Orthogonal3D; }

    ~IOrthogonal3DRouting() = 0;
};
inline IOrthogonal3DRouting::~IOrthogonal3DRouting() {}
/////////////////////////////////////////////////////////////
/// END Interface for Orthogonal 3D topology routing
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
 * \param ROUTER_ID Router identifier in the network
 * \return A method for instantiate a Routing
 */
typedef IRouting* create_Routing(sc_simcontext*,
                                 sc_module_name,
                                 unsigned short nPorts,
                                 unsigned short int ROUTER_ID);

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
