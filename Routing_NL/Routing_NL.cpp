#include "Routing_NL.h"

//#define DEBUG_ROUTING

/*!
 * \brief Routing_NL::Routing_NL West-First routing for Mesh topology constructor
 * \param mn Module name
 * \param nPorts Number of ports to route - number of requests to generate
 * \param ROUTER_ID Router identifier in the network
 */
Routing_NL::Routing_NL(sc_module_name mn,
                       unsigned short nPorts,
                       unsigned short ROUTER_ID,
                       unsigned short PORT_ID)
    : IOrthogonal2DRouting(mn,nPorts,ROUTER_ID,PORT_ID)
{
    SC_METHOD(p_REQUEST);
    sensitive << i_READ_OK << i_DATA;
}

/*!
 * \brief Routing_NL::p_REQUEST Process that generate the requests
 */
void Routing_NL::p_REQUEST() {
    UIntVar   v_DATA;                   // Used to extract fields from data
    UIntVar   v_XDEST(0,RIB_WIDTH/2);   // x-coordinate
    UIntVar   v_YDEST(0,RIB_WIDTH/2);   // y-coordinate
    bool      v_BOP;                    // packet framing bit: begin of packet
    bool      v_HEADER_PRESENT;         // A header is in the FIFO's output
    UIntVar   v_REQUEST(0,numPorts);    // Encoded request
    short int v_X_offset, v_Y_offset;   // Aux. variables used for routing

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

        if (v_Y_offset > 0) {
            if(v_X_offset < 0){
                v_REQUEST = REQ_W;
            }else{
                if(v_X_offset > 0){
                    v_REQUEST = REQ_E;
                }else{
                    v_REQUEST = REQ_N;
                }
            }
        }else{
            if(v_Y_offset < 0){
                if(v_X_offset < 0){
                    if(i_IDLE[INDEX_S]){
                        v_REQUEST = REQ_S;
                    }else{
                        if(i_IDLE[INDEX_W]){
                            v_REQUEST = REQ_W;
                        }else{
                            v_REQUEST = REQ_S;
                        }
                    }
                }else{
                    if(v_X_offset > 0){
                        if(i_IDLE[INDEX_S]){
                            v_REQUEST = REQ_S;
                        }else{
                            if(i_IDLE[INDEX_E]){
                                v_REQUEST = REQ_E;
                            }else{
                                v_REQUEST = REQ_S;
                            }
                        }
                    }else{
                        v_REQUEST = REQ_S;
                    }
                }
            }else{
                if(v_X_offset < 0){
                    v_REQUEST = REQ_W;
                }else{
                    if(v_X_offset > 0){
                        v_REQUEST = REQ_E;
                    }else{
                        v_REQUEST = REQ_L;
                    }
                }
            }
        }

        if( f.packet_ptr != NULL ) {
            f.packet_ptr->hops++;
        }
#ifdef DEBUG_ROUTING
        std::cout << "\n[Routing_NL]"
                  << " Local(" << XID << "," << YID
                  << ") - Dest(" << v_XDEST << "," << v_YDEST  << ") "
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
        } else if(v_REQUEST == 0) {
            std::cout << "NONE";
        }
        std::cout << "  @ " << sc_time_stamp() << " , ID: " << f << std::endl;
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

extern "C" {
    SS_EXP IRouting* new_Routing(sc_simcontext* simcontext,
                              sc_module_name moduleName,
                              unsigned short int nPorts,
                              unsigned short int ROUTER_ID,
                              unsigned short int PORT_ID) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new Routing_NL(moduleName,nPorts,ROUTER_ID,PORT_ID);
    }
    SS_EXP void delete_Routing(IRouting* routing) {
        delete routing;
    }
}
