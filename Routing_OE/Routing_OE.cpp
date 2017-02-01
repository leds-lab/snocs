#include "Routing_OE.h"
#include <ctime>
#include <list>
//#define DEBUG_ROUTING

/*!
 * \brief Routing_OE::Routing_OE Odd-Even routing for Mesh topology constructor
 * \param mn Module name
 * \param nPorts Number of ports to route - number of requests to generate
 * \param ROUTER_ID Router identifier in the network
 */
Routing_OE::Routing_OE(sc_module_name mn,
                       unsigned short nPorts,
                       unsigned short ROUTER_ID,
                       unsigned short PORT_ID)
    : IOrthogonal2DRouting(mn,nPorts,ROUTER_ID,PORT_ID)
{
    SC_METHOD(p_REQUEST);
    sensitive << i_READ_OK << i_DATA;
    //srand(std::time(NULL));
}

/*!
 * \brief Routing_OE::p_REQUEST Process that generate the requests
 */
void Routing_OE::p_REQUEST() {
    UIntVar   v_DATA;                   // Used to extract fields from data
    UIntVar   v_XSOURCE(0,RIB_WIDTH/2);   // x-coordinate
    UIntVar   v_YSOURCE(0,RIB_WIDTH/2);   // y-coordinate
    UIntVar   v_XDEST(0,RIB_WIDTH/2);   // x-coordinate
    UIntVar   v_YDEST(0,RIB_WIDTH/2);   // y-coordinate
    bool      v_BOP;                    // packet framing bit: begin of packet
    bool      v_HEADER_PRESENT;         // A header is in the FIFO's output
    UIntVar   v_REQUEST(0,numPorts);    // Encoded request
    short int v_X_offset, v_Y_offset;   // Aux. variables used for routing

    Flit f = i_DATA.read();
    v_DATA = f.data;

    // It extracts the RIB fields and the framing bits
    v_XSOURCE = v_DATA.range(2*RIB_WIDTH-1, 2*RIB_WIDTH - RIB_WIDTH/2);
    v_YSOURCE = v_DATA.range(2*RIB_WIDTH - RIB_WIDTH/2 - 1,RIB_WIDTH);
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
        if(v_X_offset == 0 && v_Y_offset == 0) {
            v_REQUEST = REQ_L;
        } else {
            std::vector<unsigned char> Avail_Dimension_Set;
            if(v_X_offset == 0){
                if(v_Y_offset > 0){
                    Avail_Dimension_Set.push_back(REQ_N);
                }else{
                    Avail_Dimension_Set.push_back(REQ_S);
                }
            }else{
                if(v_X_offset > 0){
                    if(v_Y_offset == 0){
                        Avail_Dimension_Set.push_back(REQ_E);
                    }else{
                        if(XID % 2 == 1 || XID == v_XSOURCE){
                            if(v_Y_offset > 0){
                                Avail_Dimension_Set.push_back(REQ_N);
                            }else{
                                Avail_Dimension_Set.push_back(REQ_S);
                            }
                        }
                        if(v_XDEST % 2 == 1|| v_X_offset != 1){
                            Avail_Dimension_Set.push_back(REQ_E);
                        }
                        if(Avail_Dimension_Set.empty())
                            std::cout << "Avail_Dimension_Set vazio!";
                    }
                }else{
                    Avail_Dimension_Set.push_back(REQ_W);
                    if(XID % 2 == 0){
                        if(v_Y_offset > 0){
                            Avail_Dimension_Set.push_back(REQ_N);
                        }else{
                            Avail_Dimension_Set.push_back(REQ_S);
                        }
                    }
                }
            }
            unsigned char v_REQ_SORTED;
            do {
                //unsigned int posSorted = rand() % Avail_Dimension_Set.size();
                v_REQ_SORTED = Avail_Dimension_Set.back();
                unsigned char v_INDEX_PORT_SORTED = log2(v_REQ_SORTED);
                if( i_IDLE[v_INDEX_PORT_SORTED].read() && v_INDEX_PORT_SORTED != PORT_ID ) {
                    break;
                } else {
                    Avail_Dimension_Set.pop_back();
                }
            } while (!Avail_Dimension_Set.empty());
            v_REQUEST = v_REQ_SORTED;
        }

#ifdef DEBUG_ROUTING
        std::cout << "\n[Routing_OE]"
                  << " Local(" << XID << "," << YID
                  << ") - Fonte(" << v_XSOURCE << "," << v_YSOURCE
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
                              unsigned short PORT_ID) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new Routing_OE(moduleName,nPorts,ROUTER_ID,PORT_ID);
    }
    SS_EXP void delete_Routing(IRouting* routing) {
        delete routing;
    }
}
