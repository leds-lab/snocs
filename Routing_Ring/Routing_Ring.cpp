#include "Routing_Ring.h"

//#define DEBUG_ROUTING

Routing_Ring::Routing_Ring(sc_module_name mn,
                           unsigned short nPorts,
                           unsigned short ROUTER_ID)
    : IRouting(mn,nPorts,ROUTER_ID),
      REQ_NONE(0),
      REQ_LOCAL(1),
      REQ_CLOCKWISE(2),
      REQ_COUNTERCLOCKWISE(4)
{
    SC_METHOD(p_REQUEST);
    sensitive << i_READ_OK << i_DATA;
}

/*!
 * \brief Routing_Ring::p_REQUEST Process that generate the requests
 */
void Routing_Ring::p_REQUEST() {
    UIntVar   v_DATA;               // Used to extract fields from data
    UIntVar   v_DEST(0,RIB_WIDTH);  // Destination address
    bool      v_BOP;                // packet framing bit: begin of packet
    bool      v_HEADER_PRESENT;     // A header is in the FIFO's output
    UIntVar   v_REQUEST(0,numPorts);// Encoded request
    int v_LOCAL, v_OFFSET;    // Aux. variables used for routing

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
        unsigned short v_LAST_ID = NUM_ELEMENTS - 1;

        v_LOCAL = ROUTER_ID;
        v_DEST = v_DATA.range(RIB_WIDTH-1, 0);

        v_OFFSET = v_DEST.to_int() - v_LOCAL;

        if (v_OFFSET != 0) {
            if (v_OFFSET > 0) {
                if( v_OFFSET > v_LAST_ID/2 ) {
                    v_REQUEST = REQ_COUNTERCLOCKWISE;
                } else {
                    v_REQUEST = REQ_CLOCKWISE;
                }
            } else {
                if( (v_OFFSET*-1) <= v_LAST_ID/2 ) {
                    v_REQUEST = REQ_COUNTERCLOCKWISE;
                } else {
                    v_REQUEST = REQ_CLOCKWISE;
                }
            }
        } else { // X == Y == 0
            v_REQUEST = REQ_LOCAL;
        }
        if( f.packet_ptr != NULL ) {
            f.packet_ptr->hops++;
        }

#ifdef DEBUG_ROUTING
        std::cout << "\n[Routing_Ring]"
                  << " LOCAL: " << v_LOCAL << ", DEST: " << v_DEST
                  << ", Req: ";
        switch(v_REQUEST.to_uint()) {
            case 1:
                std::cout << "LOCAL";
                break;
            case 2:
                std::cout << "CLOCKWISE";
                break;
            case 4:
                std::cout << "COUNTERCLOCKWISE";
                break;
            default:
                std::cout << "NONE";
        }
        UIntVar pckId = f.packet_ptr->packetId;
        std::cout << " PckId: " << pckId.to_string(SC_HEX_US,false) << " @ " << sc_time_stamp();
#endif
    } else {
        v_REQUEST = REQ_NONE;
    }

    // Outputs
    for( unsigned short i = 0; i < numPorts; i++ ) {
        o_REQUEST[i].write( v_REQUEST[i] );
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////
/*!
 * Factory Method to plugin use
 */
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

        return new Routing_Ring(moduleName,nPorts,ROUTER_ID);
    }
    SS_EXP void delete_Routing(IRouting* routing) {
        delete routing;
    }
}
