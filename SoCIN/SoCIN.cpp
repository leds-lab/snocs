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
      w_X_DATA_IN("w_X_DATA_IN"),
      w_X_VALID_IN("w_X_VALID_IN"),
      w_X_RETURN_IN("w_X_RETURN_IN"),
      w_X_DATA_OUT("w_X_DATA_OUT"),
      w_X_VALID_OUT("w_X_VALID_OUT"),
      w_X_RETURN_OUT("w_X_RETURN_OUT"),
      w_Y_DATA_IN("w_Y_DATA_IN"),
      w_Y_VALID_IN("w_Y_VALID_IN"),
      w_Y_RETURN_IN("w_Y_RETURN_IN"),
      w_Y_DATA_OUT("w_Y_DATA_OUT"),
      w_Y_VALID_OUT("w_Y_VALID_OUT"),
      w_Y_RETURN_OUT("w_Y_RETURN_OUT")
{
    unsigned short numberOfXWires = (X_SIZE-1) * Y_SIZE;
    unsigned short numberOfYWires = (Y_SIZE-1) * X_SIZE;

    // Allocating wires
    // X direction
    w_X_DATA_IN.init(numberOfXWires);
    w_X_VALID_IN.init(numberOfXWires);
    w_X_RETURN_IN.init(numberOfXWires);
    w_X_DATA_OUT.init(numberOfXWires);
    w_X_VALID_OUT.init(numberOfXWires);
    w_X_RETURN_OUT.init(numberOfXWires);
    // Y direction
    w_Y_DATA_IN.init(numberOfYWires);
    w_Y_VALID_IN.init(numberOfYWires);
    w_Y_RETURN_IN.init(numberOfYWires);
    w_Y_DATA_OUT.init(numberOfYWires);
    w_Y_VALID_OUT.init(numberOfYWires);
    w_Y_RETURN_OUT.init(numberOfYWires);

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

            std::string rName = "ParIS";
            char buff[15];
            sprintf(buff,"[%u][%u]",x,y);
            rName.append(buff);

            // Instantiating a router
            IRouter* router = PLUGIN_MANAGER->routerInstance(rName.c_str(),x,y,nPorts);

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
                router->i_DATA_IN[rPortId](w_Y_DATA_IN[yWireId]);
                router->i_VALID_IN[rPortId](w_Y_VALID_IN[yWireId]);
                router->o_RETURN_IN[rPortId](w_Y_RETURN_IN[yWireId]);
                router->o_DATA_OUT[rPortId](w_Y_DATA_OUT[yWireId]);
                router->o_VALID_OUT[rPortId](w_Y_VALID_OUT[yWireId]);
                router->i_RETURN_OUT[rPortId](w_Y_RETURN_OUT[yWireId]);
            }
            // If EAST is used, it is the port 2 in the router,
            // except in the top border routers that is the port 1,
            // because they don't have north port.
            // It is calculated with the sum of use ports North and East (clockwise sum)
            if( useEast ) {
                xWireId = routerId - y;
                rPortId = useNorth + useEast;
                router->i_DATA_IN[rPortId](w_X_DATA_IN[xWireId]);
                router->i_VALID_IN[rPortId](w_X_VALID_IN[xWireId]);
                router->o_RETURN_IN[rPortId](w_X_RETURN_IN[xWireId]);
                router->o_DATA_OUT[rPortId](w_X_DATA_OUT[xWireId]);
                router->o_VALID_OUT[rPortId](w_X_VALID_OUT[xWireId]);
                router->i_RETURN_OUT[rPortId](w_X_RETURN_OUT[xWireId]);
            }

            if( useSouth ) {
                yWireId = routerId - X_SIZE;
                rPortId = useNorth + useEast + useSouth;
                router->i_DATA_IN[rPortId](w_Y_DATA_IN[yWireId]);
                router->i_VALID_IN[rPortId](w_Y_VALID_IN[yWireId]);
                router->o_RETURN_IN[rPortId](w_Y_RETURN_IN[yWireId]);
                router->o_DATA_OUT[rPortId](w_Y_DATA_OUT[yWireId]);
                router->o_VALID_OUT[rPortId](w_Y_VALID_OUT[yWireId]);
                router->i_RETURN_OUT[rPortId](w_Y_RETURN_OUT[yWireId]);
            }

            if( useWest ) {
                xWireId = routerId - y - 1;
                rPortId = useNorth + useEast + useSouth + useWest;
                router->i_DATA_IN[rPortId](w_X_DATA_IN[xWireId]);
                router->i_VALID_IN[rPortId](w_X_VALID_IN[xWireId]);
                router->o_RETURN_IN[rPortId](w_X_RETURN_IN[xWireId]);
                router->o_DATA_OUT[rPortId](w_X_DATA_OUT[xWireId]);
                router->o_VALID_OUT[rPortId](w_X_VALID_OUT[xWireId]);
                router->i_RETURN_OUT[rPortId](w_X_RETURN_OUT[xWireId]);
            }

            u_ROUTER[routerId] = router;
        }
    }

}

SoCIN::~SoCIN() {
    u_ROUTER.clear();
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
