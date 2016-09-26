#include "Routing_XY.h"

/*!
 * \brief Routing_XY::Routing_XY XY routing for Mesh topology constructor
 * \param mn Module name
 * \param nPorts Number of ports to route - number of requests to generate
 * \param XID X
 * \param YID
 */
Routing_XY::Routing_XY(sc_module_name mn,
                       unsigned short nPorts,
                       unsigned short XID,
                       unsigned short YID)
    : IMesh2DRouting(mn,nPorts,XID,YID)
{
    SC_METHOD(p_REQUEST);
    sensitive << i_READ_OK << i_DATA;
}

/*!
 * \brief Routing_XY::p_REQUEST Process that generate the requests
 */
void Routing_XY::p_REQUEST() {

    UIntVar   v_DATA;                   // Used to extract fields from data
    UIntVar   v_XDEST(0,RIB_WIDTH/2);   // x-coordinate
    UIntVar   v_YDEST(0,RIB_WIDTH/2);   // y-coordinate
    bool      v_BOP;                    // packet framing bit: begin of packet
    bool      v_HEADER_PRESENT;         // A header is in the FIFO's output
    UIntVar   v_REQUEST(0,numPorts);    // Encoded request
    short int v_X, v_Y;                 // Aux. variables used for routing

    Flit f = i_DATA.read();
    v_DATA = f.data;

    // It extracts the RIB fields and the framing bits
    v_XDEST = v_DATA.range(RIB_WIDTH-1, RIB_WIDTH/2);
    v_YDEST = v_DATA.range(RIB_WIDTH/2-1,0);
    v_BOP   = v_DATA[FLIT_WIDTH-2];

    std::cout << "XY - Data: " << v_DATA.to_string(SC_HEX_US,false)
              << ", F.data: " << f.data.to_string(SC_HEX_US,false)
              << ", F.data.length: " << f.data.length()
              << ", XYD: " << XID << ", YID: " << YID
              << ", Xdest: " << v_XDEST << ", Ydest: " << v_YDEST << ", BOP: " << v_BOP
              << ", v_REQ.length: " << v_REQUEST.length() << std::endl;

    // It determines if a header is present
    if ((v_BOP==1) && (i_READ_OK.read()==1)) {
        v_HEADER_PRESENT = 1;
    } else {
        v_HEADER_PRESENT = 0;
    }

    // It runs the routing algorithm
    if (v_HEADER_PRESENT) {
        v_X = (int) v_XDEST.to_int() - (int) XID;
        v_Y = (int) v_YDEST.to_int() - (int) YID;
        // TODO: Verificar requisições
        if (v_X != 0) {
            if (v_X > 0) {
                v_REQUEST = REQ_E;
                std::cout << "XY - XID: " << XID << ", YID: " << YID
                          << ", v_REQ: " << v_REQUEST.to_string(SC_BIN_US,false)
                          << " - EAST" << std::endl;
            } else {
                v_REQUEST = REQ_W;
                std::cout << "XY - XID: " << XID << ", YID: " << YID
                          << ", v_REQ: " << v_REQUEST.to_string(SC_BIN_US,false)
                          << " - WEST" << std::endl;
            }
        } else if (v_Y != 0) {
            if (v_Y > 0) {
                v_REQUEST = REQ_N;
                std::cout << "XY - XID: " << XID << ", YID: " << YID
                          << ", v_REQ: " << v_REQUEST.to_string(SC_BIN_US,false)
                          << " - NORTH" << std::endl;
            } else {
                v_REQUEST = REQ_S;
                std::cout << "XY - XID: " << XID << ", YID: " << YID
                          << ", v_REQ: " << v_REQUEST.to_string(SC_BIN_US,false)
                          << " - SOUTH" << std::endl;
            }
        } else { // X == Y == 0
            v_REQUEST = REQ_L;
            std::cout << "XY - XID: " << XID << ", YID: " << YID
                      << ", v_REQ: " << v_REQUEST.to_string(SC_BIN_US,false)
                      << " - LOCAL" << std::endl;
        }
    } else {
        v_REQUEST = REQ_NONE;
        std::cout << "XY - XID: " << XID << ", YID: " << YID
                  << ", v_REQ: " << v_REQUEST.to_string(SC_BIN_US,false)
                  << " - NONE" << std::endl;
    }

    // Outputs
    for( unsigned short i = 0; i < numPorts; i++ ) {
        o_REQUEST[i].write( v_REQUEST[i] );
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
    SS_EXP IRouting* new_Routing(sc_simcontext* simcontext,
                              sc_module_name moduleName,
                              unsigned short int nPorts,
                              unsigned short int XID,
                              unsigned short int YID) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new Routing_XY(moduleName,nPorts,XID,YID);
    }
    SS_EXP void delete_Routing(IRouting* routing) {
        delete routing;
    }
}
