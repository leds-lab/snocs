#include "Routing_XYZ.h"

#define DEBUG_ROUTING

Routing_XYZ::Routing_XYZ(sc_module_name mn,
                         unsigned short nPorts,
                         unsigned short ROUTER_ID)
    : IOrthogonal3DRouting(mn,nPorts,ROUTER_ID)
{
    SC_METHOD(p_REQUEST);
    sensitive << i_READ_OK << i_DATA;
}

/*!
 * \brief Routing_XYZ::p_REQUEST
 * Routing XYZXY.
 * Routes first Xtsv, second Ytsv, third Z, fourth Xdest, fifth Ydest.
 */
void Routing_XYZ::p_REQUEST() {

    UIntVar   v_DATA;                   // Used to extract fields from data
    UIntVar   v_X_DEST(0,3);            // x destination coordinate
    UIntVar   v_Y_DEST(0,3);            // y destination coordinate
    UIntVar   v_Z_DEST(0,2);            // z destination coordinate
    UIntVar   v_X_TSV(0,3);             // x through silicon via coordinate
    UIntVar   v_Y_TSV(0,3);             // y through silicon via coordinate
    bool      v_BOP;                    // packet framing bit: begin of packet
    bool      v_HEADER_PRESENT;         // A header is in the FIFO's output
    UIntVar   v_REQUEST(0,numPorts);    // Encoded request
    short int v_X_TSV_offset,v_Y_TSV_offset,
            v_X_offset, v_Y_offset,v_Z_offset;   // Aux. variables used for routing

    Flit f = i_DATA.read();
    v_DATA = f.data;

    // It extracts the RIB fields and the framing bits
    v_BOP   = v_DATA[FLIT_WIDTH-2];

    // It determines if a header is present
    if ((v_BOP==1) && (i_READ_OK.read()==1)) {
        v_HEADER_PRESENT = 1;
    } else {
        v_HEADER_PRESENT = 0;
    }

    // It runs the routing algorithm
    if (v_HEADER_PRESENT) {
        // Extract the X, Y and Z address
        v_X_TSV  = v_DATA.range(21,19);
        v_Y_TSV  = v_DATA.range(18,16);
        v_X_DEST = v_DATA.range(7,5);
        v_Y_DEST = v_DATA.range(4,2);
        v_Z_DEST = v_DATA.range(1,0);

        v_X_TSV_offset = v_X_TSV.to_int() - (int) XID;
        v_Y_TSV_offset = v_Y_TSV.to_int() - (int) YID;

        v_X_offset = (int) v_X_DEST.to_int() - (int) XID;
        v_Y_offset = (int) v_Y_DEST.to_int() - (int) YID;
        v_Z_offset = (int) v_Z_DEST.to_int() - (int) ZID;

        if (v_X_TSV_offset != 0) { // First X tsv
            if (v_X_TSV_offset > 0) {
                v_REQUEST = REQ_E;
            } else {
                v_REQUEST = REQ_W;
            }
        } else if (v_Y_TSV_offset != 0) { // Second Y tsv
            if (v_Y_TSV_offset > 0) {
                v_REQUEST = REQ_N;
            } else {
                v_REQUEST = REQ_S;
            }
        } else if(v_Z_offset != 0) { // Third Z
            if(v_Z_offset > 0) {
                v_REQUEST = REQ_U;
            } else {
                v_REQUEST = REQ_D;
            }
        } else if (v_X_offset != 0) { // Fourth X
            if (v_X_offset > 0) {
                v_REQUEST = REQ_E;
            } else {
                v_REQUEST = REQ_W;
            }
        } else if (v_Y_offset != 0) { // Fifth Y
            if (v_Y_offset > 0) {
                v_REQUEST = REQ_N;
            } else {
                v_REQUEST = REQ_S;
            }
        } else { // X == Y == Z == 0
            v_REQUEST = REQ_L;
        }
    } else {
        v_REQUEST = REQ_NONE;
    }
#ifdef DEBUG_ROUTING
        std::cout << "\n[Routing_XYZ]"
                  << " Local(" << XID << "," << YID << "," << ZID
                  << ") -> DestTSV(" << v_X_TSV << "," << v_Y_TSV
                  << ") - Dest(" << v_X_DEST << "," << v_Y_DEST << "," << v_Z_DEST << ") "
                  << ", Req: ";
        if(v_REQUEST == REQ_L) {
            std::cout << "LOCAL";
        } else if(v_REQUEST == REQ_N) {
            std::cout << "NORTH";
        } else if(v_REQUEST == REQ_E) {
            std::cout << "EAST";
        } else if(v_REQUEST == REQ_S) {
            std::cout << "SOUTH";
        } else if(v_REQUEST == REQ_W) {
            std::cout << "WEST";
        } else if(v_REQUEST == REQ_U) {
            std::cout << "UP";
        } else if(v_REQUEST == REQ_D) {
            std::cout << "DOWN";
        }
#endif
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
                              unsigned short int ROUTER_ID) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new Routing_XYZ(moduleName,nPorts,ROUTER_ID);
    }
    SS_EXP void delete_Routing(IRouting* routing) {
        delete routing;
    }
}
