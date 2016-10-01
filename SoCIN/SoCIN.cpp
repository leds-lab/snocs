#include "SoCIN.h"
#include "../Router/Router.h"
#include "../PluginManager/PluginManager.h"

#define COORDINATE_TO_ID(x,y) (y * X_SIZE + x)

///////////////////////////////////////////////////////////////
/// \brief SoCIN::SoCIN Constructor that instantiate the routers
/// and made the connections between the routers in a Mesh 2D
/// topology.
/// \param mn Module Name
///
SoCIN::SoCIN(sc_module_name mn)
    : INoC(mn,(X_SIZE * Y_SIZE)),
      w_X_DATA_TO_LEFT("w_X_DATA_IN"),
      w_X_VALID_TO_LEFT("w_X_VALID_IN"),
      w_X_RETURN_TO_LEFT("w_X_RETURN_IN"),
      w_X_DATA_TO_RIGHT("w_X_DATA_OUT"),
      w_X_VALID_TO_RIGHT("w_X_VALID_OUT"),
      w_X_RETURN_TO_RIGHT("w_X_RETURN_OUT"),
      w_Y_DATA_TO_SOUTH("w_Y_DATA_IN"),
      w_Y_VALID_TO_SOUTH("w_Y_VALID_IN"),
      w_Y_RETURN_TO_SOUTH("w_Y_RETURN_IN"),
      w_Y_DATA_TO_NORTH("w_Y_DATA_OUT"),
      w_Y_VALID_TO_NORTH("w_Y_VALID_OUT"),
      w_Y_RETURN_TO_NORTH("w_Y_RETURN_OUT")
{
    unsigned short numberOfXWires = (X_SIZE-1) * Y_SIZE;
    unsigned short numberOfYWires = (Y_SIZE-1) * X_SIZE;

    // Allocating wires
    // X direction
    w_X_DATA_TO_LEFT.init(numberOfXWires);
    w_X_VALID_TO_LEFT.init(numberOfXWires);
    w_X_RETURN_TO_LEFT.init(numberOfXWires);
    w_X_DATA_TO_RIGHT.init(numberOfXWires);
    w_X_VALID_TO_RIGHT.init(numberOfXWires);
    w_X_RETURN_TO_RIGHT.init(numberOfXWires);
    // Y direction
    w_Y_DATA_TO_SOUTH.init(numberOfYWires);
    w_Y_VALID_TO_SOUTH.init(numberOfYWires);
    w_Y_RETURN_TO_SOUTH.init(numberOfYWires);
    w_Y_DATA_TO_NORTH.init(numberOfYWires);
    w_Y_VALID_TO_NORTH.init(numberOfYWires);
    w_Y_RETURN_TO_NORTH.init(numberOfYWires);

    unsigned short x,y, // Aux. to identify the router id in cartesian coordinates
        nPorts,         // Aux. to calculate the number of ports
        routerId,       // Aux. to convert cartesian coordinate to number router id
        rPortId,        // Aux. to identify the port id to be binded in connections between routers
        xWireId,yWireId;// Aux. to identify the wires id to binding connections between routers

    unsigned short useLocal = 1, // Always use Local
            useNorth, useEast, useSouth, useWest; // Aux. to identify which port must be used according the network position

    for( x = 0; x < X_SIZE; x++ ) {
        for( y = 0; y < Y_SIZE; y++ ) {
            // Set the use of ports according with the network position
            useNorth = ( y < Y_SIZE-1 ) ? 1 : 0;   // North port is not used on the top    border
            useEast  = ( x < X_SIZE-1 ) ? 1 : 0;   // East  port is not used on the right  border
            useSouth = ( y > 0 ) ? 1 : 0;          // South port is not used on the botton border
            useWest  = ( x > 0 ) ? 1 : 0;          // West  port is not used on the left   border

            // Calculate the number of ports needed to the router to be instantiated
            nPorts = useLocal + useNorth + useEast + useSouth + useWest;

            routerId = COORDINATE_TO_ID(x,y);

            char rName[15];
            sprintf(rName,"ParIS[%u][%u]",x,y);

            // Instantiating a router
            IRouter* router = PLUGIN_MANAGER->routerInstance(rName,x,y,nPorts);

            // Binding ports of the router
            // System signals
            router->i_CLK(i_CLK);
            router->i_RST(i_RST);
            // External interface
            // Port 0 is always the LOCAL port - core communication
            router->i_DATA_IN[0](i_DATA_IN[routerId]);
            router->i_VALID_IN[0](i_VALID_IN[routerId]);
            router->o_RETURN_IN[0](o_RETURN_IN[routerId]);
            router->o_DATA_OUT[0](o_DATA_OUT[routerId]);
            router->o_VALID_OUT[0](o_VALID_OUT[routerId]);
            router->i_RETURN_OUT[0](i_RETURN_OUT[routerId]);

            // Connections with adjacent/neighbour routers
            // If NORTH is used (NORTH=1), it is always the port 1 in the router
            if( useNorth ) {
                yWireId = routerId; // Always the same id of the router
                rPortId = useNorth;
                router->i_DATA_IN[rPortId](w_Y_DATA_TO_SOUTH[yWireId]);
                router->i_VALID_IN[rPortId](w_Y_VALID_TO_SOUTH[yWireId]);
                router->o_RETURN_IN[rPortId](w_Y_RETURN_TO_SOUTH[yWireId]);
                router->o_DATA_OUT[rPortId](w_Y_DATA_TO_NORTH[yWireId]);
                router->o_VALID_OUT[rPortId](w_Y_VALID_TO_NORTH[yWireId]);
                router->i_RETURN_OUT[rPortId](w_Y_RETURN_TO_NORTH[yWireId]);
            }
            // If EAST is used, it is the port 2 in the router,
            // except in the top border routers that is the port 1,
            // because they don't have north port.
            // It is calculated with the sum of use ports North and East (clockwise sum)
            if( useEast ) {
                xWireId = routerId - y;
                rPortId = useNorth + useEast;
                router->i_DATA_IN[rPortId](w_X_DATA_TO_LEFT[xWireId]);
                router->i_VALID_IN[rPortId](w_X_VALID_TO_LEFT[xWireId]);
                router->o_RETURN_IN[rPortId](w_X_RETURN_TO_LEFT[xWireId]);
                router->o_DATA_OUT[rPortId](w_X_DATA_TO_RIGHT[xWireId]);
                router->o_VALID_OUT[rPortId](w_X_VALID_TO_RIGHT[xWireId]);
                router->i_RETURN_OUT[rPortId](w_X_RETURN_TO_RIGHT[xWireId]);
            }

            if( useSouth ) {
                yWireId = routerId - X_SIZE;
                rPortId = useNorth + useEast + useSouth;
                router->i_DATA_IN[rPortId](w_Y_DATA_TO_NORTH[yWireId]);
                router->i_VALID_IN[rPortId](w_Y_VALID_TO_NORTH[yWireId]);
                router->o_RETURN_IN[rPortId](w_Y_RETURN_TO_NORTH[yWireId]);
                router->o_DATA_OUT[rPortId](w_Y_DATA_TO_SOUTH[yWireId]);
                router->o_VALID_OUT[rPortId](w_Y_VALID_TO_SOUTH[yWireId]);
                router->i_RETURN_OUT[rPortId](w_Y_RETURN_TO_SOUTH[yWireId]);
            }

            if( useWest ) {
                xWireId = routerId - y - 1;
                rPortId = useNorth + useEast + useSouth + useWest;
                router->i_DATA_IN[rPortId](w_X_DATA_TO_RIGHT[xWireId]);
                router->i_VALID_IN[rPortId](w_X_VALID_TO_RIGHT[xWireId]);
                router->o_RETURN_IN[rPortId](w_X_RETURN_TO_RIGHT[xWireId]);
                router->o_DATA_OUT[rPortId](w_X_DATA_TO_LEFT[xWireId]);
                router->o_VALID_OUT[rPortId](w_X_VALID_TO_LEFT[xWireId]);
                router->i_RETURN_OUT[rPortId](w_X_RETURN_TO_LEFT[xWireId]);
            }

            u_ROUTER[routerId] = router;
        }
    }

#ifdef DEBUG_SOCIN
    SC_METHOD(p_DEBUG);
    sensitive << i_CLK.pos();
#endif

#ifdef WAVEFORM_SOCIN
    tf = sc_create_vcd_trace_file("socin");
    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,i_RST,"RST");
    unsigned short i;
    // Interfaces - routers
    for( i = 0; i < numRouters; i++ ) {
        char strI[5];
        sprintf(strI,"(%u)",i);
        char strDataIn[15];
        sprintf(strDataIn,"DATA_IN%s",strI);
        sc_trace(tf, i_DATA_IN[i],strDataIn);
        char strValidIn[15];
        sprintf(strValidIn,"VALID_IN%s",strI);
        sc_trace(tf, i_VALID_IN[i],strValidIn);
        char strRetIn[15];
        sprintf(strRetIn,"RET_IN%s",strI);
        sc_trace(tf, o_RETURN_IN[i],strRetIn);

        char strDataOut[15];
        sprintf(strDataOut,"DATA_OUT%s",strI);
        sc_trace(tf, o_DATA_OUT[i],strDataOut);
        char strValidOut[15];
        sprintf(strValidOut,"VALID_OUT%s",strI);
        sc_trace(tf, o_VALID_OUT[i],strValidOut);
        char strRetOut[15];
        sprintf(strRetOut,"RET_OUT%s",strI);
        sc_trace(tf, i_RETURN_OUT[i],strRetOut);
    }

    // X wires
    for( i = 0; i < numberOfXWires; i++) {
        char strI[5];
        sprintf(strI,"(%u)",i);

        char wXDataLeft[20];
        sprintf(wXDataLeft,"X-Data-to-Left%s",strI);
        sc_trace(tf,w_X_DATA_TO_LEFT[i],wXDataLeft);

        char wXValidLeft[21];
        sprintf(wXValidLeft,"X-Valid-to-Left%s",strI);
        sc_trace(tf,w_X_VALID_TO_LEFT[i],wXValidLeft);

        char wXRetLeft[21];
        sprintf(wXRetLeft,"X-Ret-to-Left%s",strI);
        sc_trace(tf,w_X_RETURN_TO_LEFT[i],wXRetLeft);

        char wXDataRight[21];
        sprintf(wXDataRight,"X-Data-to-Right%s",strI);
        sc_trace(tf,w_X_DATA_TO_RIGHT[i],wXDataRight);

        char wXValidRight[22];
        sprintf(wXValidRight,"X-Valid-to-Right%s",strI);
        sc_trace(tf,w_X_VALID_TO_RIGHT[i],wXValidRight);

        char wXRetRight[22];
        sprintf(wXRetRight,"X-Ret-to-Right%s",strI);
        sc_trace(tf,w_X_RETURN_TO_RIGHT[i],wXRetRight);
    }
    // Y wires
    for( i = 0; i < numberOfYWires; i++) {
        char strI[5];
        sprintf(strI,"(%u)",i);

        char wYDataSouth[21];
        sprintf(wYDataSouth,"Y-Data-to-South%s",strI);
        sc_trace(tf,w_Y_DATA_TO_SOUTH[i],wYDataSouth);

        char wYValidSouth[22];
        sprintf(wYValidSouth,"Y-Valid-to-South%s",strI);
        sc_trace(tf,w_Y_VALID_TO_SOUTH[i],wYValidSouth);

        char wYRetSouth[22];
        sprintf(wYRetSouth,"Y-Ret-to-South%s",strI);
        sc_trace(tf,w_Y_RETURN_TO_SOUTH[i],wYRetSouth);

        char wYDataNorth[21];
        sprintf(wYDataNorth,"Y-Data-to-North%s",strI);
        sc_trace(tf,w_Y_DATA_TO_NORTH[i],wYDataNorth);

        char wYValidNorth[22];
        sprintf(wYValidNorth,"Y-Valid-to-North%s",strI);
        sc_trace(tf,w_Y_VALID_TO_NORTH[i],wYValidNorth);

        char wYRetNorth[22];
        sprintf(wYRetNorth,"Y-Ret-to-North%s",strI);
        sc_trace(tf,w_Y_RETURN_TO_NORTH[i],wYRetNorth);
    }

#endif
}

SoCIN::~SoCIN() {
#ifdef WAVEFORM_SOCIN
    sc_close_vcd_trace_file(tf);
#endif
    u_ROUTER.clear();
}

void SoCIN::p_DEBUG() {

    // Wire[0] R[0][0] - [1][0]
    unsigned short wireId = 0;

    Flit dataToRight = w_X_DATA_TO_RIGHT[wireId].read();
    bool retToRight = w_X_RETURN_TO_RIGHT[wireId].read();
    bool valToRight = w_X_VALID_TO_RIGHT[wireId].read();

    Flit dataToLeft = w_X_DATA_TO_LEFT[wireId].read();
    bool retToLeft = w_X_RETURN_TO_LEFT[wireId].read();
    bool valToLeft = w_X_VALID_TO_LEFT[wireId].read();

    printf("\n-->[SoCIN] @ %s wX_ID[%u] Data->: %s, Ret->: %d, Val->: %d, Data<-: %s, Ret<-: %d, Val<-: %d",
           sc_time_stamp().to_string().c_str(),wireId,
           dataToRight.data.to_string(SC_HEX_US).c_str(),
           retToRight,valToRight,
           dataToLeft.data.to_string(SC_HEX_US).c_str(),
           retToLeft,valToLeft);

}

////////////////////////////////////////////////////////////////////////////////
/*!
 * Factory Methods to instantiation and deallocation
 */
extern "C" {
    SS_EXP INoC* new_NoC(sc_simcontext* simcontext,sc_module_name moduleName) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new SoCIN(moduleName);
    }
    SS_EXP void delete_NoC(INoC* noc) {
        delete noc;
    }
}
