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
    : IRouting(mn,nPorts,XID,YID)
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
              << ", Xdest: " << v_XDEST << ", Ydest: " << v_YDEST << ", BOP: " << v_BOP << std::endl;

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

        // TODO: Continuar daqui
        // TODO: Mapear requisições
        if (v_X != 0) {
            if (v_X > 0) {
                v_REQUEST = REQ_E;
            } else {
                v_REQUEST = REQ_W;
            }
        } else if (v_Y != 0) {
            if (v_Y > 0) {
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
