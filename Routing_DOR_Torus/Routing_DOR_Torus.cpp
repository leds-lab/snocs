#include "Routing_DOR_Torus.h"


Routing_DOR_Torus::Routing_DOR_Torus(sc_module_name mn,
                                     unsigned short nPorts,
                                     unsigned short XID,
                                     unsigned short YID)
    : IOrthogonal2DRouting(mn,nPorts,XID,YID)
{
    SC_METHOD(p_REQUEST);
    sensitive << i_READ_OK << i_DATA;
}

/*!
 * \brief Routing_DOR_Torus::p_REQUEST Process that generate the requests
 */
void Routing_DOR_Torus::p_REQUEST() {
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
                if( v_X_offset > (X_SIZE-1)/2 ) {
                    v_REQUEST = REQ_W;
                } else {
                    v_REQUEST = REQ_E;
                }
            } else {
                if( (v_X_offset*-1) <= (X_SIZE-1)/2 ) {
                    v_REQUEST = REQ_W;
                } else {
                    v_REQUEST = REQ_E;
                }
            }
        } else if (v_Y_offset != 0) {
            if (v_Y_offset > 0) {
                if( v_Y_offset > (Y_SIZE-1)/2 ) {
                    v_REQUEST = REQ_S;
                } else {
                    v_REQUEST = REQ_N;
                }
            } else {
                if( (v_Y_offset*-1) <= (Y_SIZE-1)/2 ) {
                    v_REQUEST = REQ_S;
                } else {
                    v_REQUEST = REQ_N;
                }
            }
        } else { // X == Y == 0
            v_REQUEST = REQ_L;
        }
        std::cout << "\n[DOR_TORUS] XID: " << XID << ", YID: " << YID
                  << ", xDest: " << v_XDEST << ", yDest: " << v_YDEST
                  << ", Req: ";
        switch(v_REQUEST.to_uint()) {
            case 1:
                std::cout << "LOCAL";
                break;
            case 2:
                std::cout << "NORTH";
                break;
            case 4:
                std::cout << "EAST";
                break;
            case 8:
                std::cout << "SOUTH";
                break;
            case 16:
                std::cout << "WEST";
                break;
            default:
                std::cout << "NONE";
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

        return new Routing_DOR_Torus(moduleName,nPorts,XID,YID);
    }
    SS_EXP void delete_Routing(IRouting* routing) {
        delete routing;
    }
}
