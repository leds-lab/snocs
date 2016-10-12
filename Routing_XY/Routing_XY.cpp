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
    : IOrthogonal2DRouting(mn,nPorts,XID,YID)
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
    short int v_X_offset, v_Y_offset;                 // Aux. variables used for routing

    Flit f = i_DATA.read();
    v_DATA = f.data;

    // It extracts the RIB fields and the framing bits
    v_XDEST = v_DATA.range(RIB_WIDTH-1, RIB_WIDTH/2);
    v_YDEST = v_DATA.range(RIB_WIDTH/2-1,0);
    v_BOP   = v_DATA[FLIT_WIDTH-2];

    // It determines if a header is present
    if ((v_BOP==1) && (i_READ_OK.read()==1)) {
        v_HEADER_PRESENT = 1;
    } else {
        v_HEADER_PRESENT = 0;
    }

    // It runs the routing algorithm
    if (v_HEADER_PRESENT) {
        v_X_offset = (int) v_XDEST.to_int() - (int) XID;
        v_Y_offset = (int) v_YDEST.to_int() - (int) YID;
        if (v_X_offset != 0) {
            if (v_X_offset > 0) {
                v_REQUEST = REQ_E;
            } else {
                v_REQUEST = REQ_W;
            }
        } else if (v_Y_offset != 0) {
            if (v_Y_offset > 0) {
                v_REQUEST = REQ_N;
            } else {
                v_REQUEST = REQ_S;
            }
        } else { // X == Y == 0
            v_REQUEST = REQ_L;
        }
    } else {
        v_REQUEST = REQ_NONE;
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
