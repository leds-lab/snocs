#include "SoCIN_3D.h"
#include "../Router/Router.h"
#include "../PluginManager/PluginManager.h"

#include <stdexcept>

//#define WAVEFORM_SOCIN

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// SoCINfp with virtual channels /////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/// \brief SoCIN_3D::SoCIN_3D Constructor that instantiate the routers
/// and made the connections between the routers in a Mesh 3D topology with virtual channels
/// \param mn Module Name
///
SoCIN_3D::SoCIN_3D(sc_module_name mn)
    : INoC_VC(mn,(X_SIZE * Y_SIZE * Z_SIZE),NUM_VC),
      w_X_DATA_TO_LEFT("w_X_DATA_TO_LEFT"),
      w_X_VALID_TO_LEFT("w_X_VALID_TO_LEFT"),
      w_X_RETURN_TO_LEFT("w_X_RETURN_TO_LEFT"),
      w_X_VC_SELECTOR_TO_LEFT("w_X_VC_SELECTOR_TO_LEFT"),
      w_X_DATA_TO_RIGHT("w_X_DATA_TO_RIGHT"),
      w_X_VALID_TO_RIGHT("w_X_VALID_TO_RIGHT"),
      w_X_RETURN_TO_RIGHT("w_X_RETURN_TO_RIGHT"),
      w_X_VC_SELECTOR_TO_RIGHT("w_X_VC_SELECTOR_TO_RIGHT"),
      w_Y_DATA_TO_SOUTH("w_Y_DATA_TO_SOUTH"),
      w_Y_VALID_TO_SOUTH("w_Y_VALID_TO_SOUTH"),
      w_Y_RETURN_TO_SOUTH("w_Y_RETURN_TO_SOUTH"),
      w_Y_VC_SELECTOR_TO_SOUTH("w_Y_VC_SELECTOR_TO_SOUTH"),
      w_Y_DATA_TO_NORTH("w_Y_DATA_TO_NORTH"),
      w_Y_VALID_TO_NORTH("w_Y_VALID_TO_NORTH"),
      w_Y_RETURN_TO_NORTH("w_Y_RETURN_TO_NORTH"),
      w_Y_VC_SELECTOR_TO_NORTH("w_Y_VC_SELECTOR_TO_NORTH"),
      w_Z_DATA_TO_UP("w_Z_DATA_TO_UP"),
      w_Z_VALID_TO_UP("w_Z_VALID_TO_UP"),
      w_Z_RETURN_TO_UP("w_Z_RETURN_TO_UP"),
      w_Z_VC_SELECTOR_TO_UP("w_Z_VC_SELECTOR_TO_UP"),
      w_Z_DATA_TO_DOWN("w_Z_DATA_TO_DOWN"),
      w_Z_VALID_TO_DOWN("w_Z_VALID_TO_DOWN"),
      w_Z_RETURN_TO_DOWN("w_Z_RETURN_TO_DOWN"),
      w_Z_VC_SELECTOR_TO_DOWN("w_Z_VC_SELECTOR_TO_DOWN")
{
    if(numInterfaces == 0) {
        throw std::runtime_error("Number of interfaces is 0, verify X, Y and Z sizes.");
    }

    unsigned short i; // Loop iterator

    // Allocating the number of routers needed
    u_ROUTER.resize( numInterfaces , NULL);
    unsigned short numberOfXWires = (X_SIZE-1) * Y_SIZE * Z_SIZE;
    unsigned short numberOfYWires = (Y_SIZE-1) * X_SIZE * Z_SIZE;
    unsigned short numberOfZWires = (Z_SIZE-1) * X_SIZE * Y_SIZE;

    // Allocating wires
    // X direction
    w_X_DATA_TO_LEFT.init(numberOfXWires);
    w_X_VALID_TO_LEFT.init(numberOfXWires);
    w_X_RETURN_TO_LEFT.init(numberOfXWires);
    w_X_DATA_TO_RIGHT.init(numberOfXWires);
    w_X_VALID_TO_RIGHT.init(numberOfXWires);
    w_X_RETURN_TO_RIGHT.init(numberOfXWires);
    if( NUM_VC > 1 ) {
        w_X_VC_SELECTOR_TO_LEFT.init(numberOfXWires);
        w_X_VC_SELECTOR_TO_RIGHT.init(numberOfXWires);
        for( i = 0; i < numberOfXWires; i++) {
            w_X_VC_SELECTOR_TO_LEFT[i].init(widthVcSelector);
            w_X_VC_SELECTOR_TO_RIGHT[i].init(widthVcSelector);
        }
    }
    // Y direction
    w_Y_DATA_TO_SOUTH.init(numberOfYWires);
    w_Y_VALID_TO_SOUTH.init(numberOfYWires);
    w_Y_RETURN_TO_SOUTH.init(numberOfYWires);
    w_Y_DATA_TO_NORTH.init(numberOfYWires);
    w_Y_VALID_TO_NORTH.init(numberOfYWires);
    w_Y_RETURN_TO_NORTH.init(numberOfYWires);
    if( NUM_VC > 1 ) {
        w_Y_VC_SELECTOR_TO_SOUTH.init(numberOfYWires);
        w_Y_VC_SELECTOR_TO_NORTH.init(numberOfYWires);
        for( i = 0; i < numberOfYWires; i++) {
            w_Y_VC_SELECTOR_TO_SOUTH[i].init(widthVcSelector);
            w_Y_VC_SELECTOR_TO_NORTH[i].init(widthVcSelector);
        }
    }
    // Z direction
    w_Z_DATA_TO_UP.init(numberOfZWires);
    w_Z_VALID_TO_UP.init(numberOfZWires);
    w_Z_RETURN_TO_UP.init(numberOfZWires);
    w_Z_DATA_TO_DOWN.init(numberOfZWires);
    w_Z_VALID_TO_DOWN.init(numberOfZWires);
    w_Z_RETURN_TO_DOWN.init(numberOfZWires);
    if( NUM_VC > 1 ) {
        w_Z_VC_SELECTOR_TO_UP.init(numberOfZWires);
        w_Z_VC_SELECTOR_TO_DOWN.init(numberOfZWires);
        for( i = 0; i < numberOfZWires; i++) {
            w_Z_VC_SELECTOR_TO_UP[i].init(widthVcSelector);
            w_Z_VC_SELECTOR_TO_DOWN[i].init(widthVcSelector);
        }
    }

    unsigned short x,y,z, // Aux. to identify the router id in 3D cartesian coordinates
        nPorts,           // Aux. to calculate the number of ports
        routerId,         // Aux. to convert cartesian coordinate to number router id
        rPortId,          // Aux. to identify the port id to be binded in connections between routers
        xWireId,yWireId,zWireId;  // Aux. to identify the wires id to binding connections between routers

    unsigned short useLocal = 1, // Always use Local
            useNorth, useEast, useSouth, useWest,useUp,useDown; // Aux. to identify which port must be used according the network position

    for( x = 0; x < X_SIZE; x++ ) {
        for( y = 0; y < Y_SIZE; y++ ) {
            for( z = 0; z < Z_SIZE; z++ ) {
                // Set the use of ports according with the network position
                useNorth = ( y < Y_SIZE-1 ) ? 1 : 0;   // North port is not used on the top    border
                useEast  = ( x < X_SIZE-1 ) ? 1 : 0;   // East  port is not used on the right  border
                useSouth = ( y > 0 ) ? 1 : 0;          // South port is not used on the botton border
                useWest  = ( x > 0 ) ? 1 : 0;          // West  port is not used on the left   border
                useUp    = ( z < Z_SIZE-1 ) ? 1 : 0;   // Up port is not used on the upper layer
                useDown  = ( z > 0 ) ? 1 : 0;          // Down port is not used on the bottom layer

                // Calculate the number of ports needed to the router to be instantiated
                nPorts = useLocal + useNorth + useEast + useSouth + useWest + useUp + useDown;

                routerId = COORDINATE_3D_TO_ID(x,y,z);

                char rName[20];
                sprintf(rName,"ParIS[%u][%u][%u]",x,y,z);

                // Instantiating a router
                IRouter* router = PLUGIN_MANAGER->routerInstance(rName,routerId,nPorts,NUM_VC);
                if( router == NULL ) {
                    throw std::runtime_error("[SoCIN-3D] -- ERROR: It was not possible instantiate a router.");
                }
                IRouter_VC* router_VC = dynamic_cast<IRouter_VC *>(router);

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
                if( NUM_VC > 1 ) {
                    if( router_VC != NULL ) {
                        router_VC->i_VC_IN[0](i_VC_SELECTOR[routerId]);
                        router_VC->o_VC_OUT[0](o_VC_SELECTOR[routerId]);
                    } else {
                        throw std::runtime_error("[SoCIN-3D] -- ERROR: The router instantiated is not a VC router.");
                    }
                }

                // Connections with adjacent/neighbour routers
                // If NORTH is used (NORTH=1), it is always the port 1 in the router
                if( useNorth ) {
                    yWireId = routerId - (X_SIZE*z); // Always the same id of the router
                    rPortId = useNorth;
                    router_VC->i_DATA_IN[rPortId](w_Y_DATA_TO_SOUTH[yWireId]);
                    router_VC->i_VALID_IN[rPortId](w_Y_VALID_TO_SOUTH[yWireId]);
                    router_VC->o_RETURN_IN[rPortId](w_Y_RETURN_TO_SOUTH[yWireId]);
                    router_VC->o_DATA_OUT[rPortId](w_Y_DATA_TO_NORTH[yWireId]);
                    router_VC->o_VALID_OUT[rPortId](w_Y_VALID_TO_NORTH[yWireId]);
                    router_VC->i_RETURN_OUT[rPortId](w_Y_RETURN_TO_NORTH[yWireId]);
                    if( NUM_VC > 1 ) {
                        if( router_VC != NULL ) {
                            router_VC->i_VC_IN[rPortId](i_VC_SELECTOR[yWireId]);
                            router_VC->o_VC_OUT[rPortId](o_VC_SELECTOR[yWireId]);
                        } else {
                            throw std::runtime_error("[SoCIN-3D] -- ERROR: The router instantiated is not a VC router.");
                        }
                    }
                }
                // If EAST is used, it is the port 2 in the router,
                // except in the top border routers that is the port 1,
                // because they don't have north port.
                // It is calculated with the sum of use ports North and East (clockwise sum)
                if( useEast ) {
                    xWireId = routerId - y - (Y_SIZE*z);
                    rPortId = useNorth + useEast;
                    router_VC->i_DATA_IN[rPortId](w_X_DATA_TO_LEFT[xWireId]);
                    router_VC->i_VALID_IN[rPortId](w_X_VALID_TO_LEFT[xWireId]);
                    router_VC->o_RETURN_IN[rPortId](w_X_RETURN_TO_LEFT[xWireId]);
                    router_VC->o_DATA_OUT[rPortId](w_X_DATA_TO_RIGHT[xWireId]);
                    router_VC->o_VALID_OUT[rPortId](w_X_VALID_TO_RIGHT[xWireId]);
                    router_VC->i_RETURN_OUT[rPortId](w_X_RETURN_TO_RIGHT[xWireId]);
                    if( NUM_VC > 1 ) {
                        if( router_VC != NULL ) {
                            router_VC->i_VC_IN[rPortId](w_X_VC_SELECTOR_TO_LEFT[xWireId]);
                            router_VC->o_VC_OUT[rPortId](w_X_VC_SELECTOR_TO_RIGHT[xWireId]);
                        } else {
                            throw std::runtime_error("[SoCIN-3D] -- ERROR: The router instantiated is not a VC router.");
                        }
                    }

                }

                if( useSouth ) {
                    yWireId = routerId - X_SIZE - (X_SIZE*z);
                    rPortId = useNorth + useEast + useSouth;
                    router_VC->i_DATA_IN[rPortId](w_Y_DATA_TO_NORTH[yWireId]);
                    router_VC->i_VALID_IN[rPortId](w_Y_VALID_TO_NORTH[yWireId]);
                    router_VC->o_RETURN_IN[rPortId](w_Y_RETURN_TO_NORTH[yWireId]);
                    router_VC->o_DATA_OUT[rPortId](w_Y_DATA_TO_SOUTH[yWireId]);
                    router_VC->o_VALID_OUT[rPortId](w_Y_VALID_TO_SOUTH[yWireId]);
                    router_VC->i_RETURN_OUT[rPortId](w_Y_RETURN_TO_SOUTH[yWireId]);
                    if(NUM_VC > 1) {
                        if( router_VC != NULL) {
                            router_VC->i_VC_IN[rPortId](w_Y_VC_SELECTOR_TO_NORTH[yWireId]);
                            router_VC->o_VC_OUT[rPortId](w_Y_VC_SELECTOR_TO_SOUTH[yWireId]);
                        } else {
                            throw std::runtime_error("[SoCIN-3D] -- ERROR: The router instantiated is not a VC router.");
                        }
                    }
                }

                if( useWest ) {
                    xWireId = routerId - y - 1 - (Y_SIZE*z);
                    rPortId = useNorth + useEast + useSouth + useWest;
                    router_VC->i_DATA_IN[rPortId](w_X_DATA_TO_RIGHT[xWireId]);
                    router_VC->i_VALID_IN[rPortId](w_X_VALID_TO_RIGHT[xWireId]);
                    router_VC->o_RETURN_IN[rPortId](w_X_RETURN_TO_RIGHT[xWireId]);
                    router_VC->o_DATA_OUT[rPortId](w_X_DATA_TO_LEFT[xWireId]);
                    router_VC->o_VALID_OUT[rPortId](w_X_VALID_TO_LEFT[xWireId]);
                    router_VC->i_RETURN_OUT[rPortId](w_X_RETURN_TO_LEFT[xWireId]);
                    if( NUM_VC > 1 ) {
                        if( router_VC != NULL ) {
                            router_VC->i_VC_IN[rPortId](w_X_VC_SELECTOR_TO_RIGHT[xWireId]);
                            router_VC->o_VC_OUT[rPortId](w_X_VC_SELECTOR_TO_LEFT[xWireId]);
                        } else {
                            throw std::runtime_error("[SoCIN-3D] -- ERROR: The router instantiated is not a VC router.");
                        }
                    }
                }

                if( useUp ) {
                    zWireId =  routerId;
                    rPortId = useNorth + useEast + useSouth + useWest + useUp;
                    router_VC->i_DATA_IN[rPortId](w_Z_DATA_TO_DOWN[zWireId]);
                    router_VC->i_VALID_IN[rPortId](w_Z_VALID_TO_DOWN[zWireId]);
                    router_VC->o_RETURN_IN[rPortId](w_Z_RETURN_TO_DOWN[zWireId]);
                    router_VC->o_DATA_OUT[rPortId](w_Z_DATA_TO_UP[zWireId]);
                    router_VC->o_VALID_OUT[rPortId](w_Z_VALID_TO_UP[zWireId]);
                    router_VC->i_RETURN_OUT[rPortId](w_Z_RETURN_TO_UP[zWireId]);
                    if( NUM_VC > 1 ) {
                        if( router_VC != NULL ) {
                            router_VC->i_VC_IN[rPortId](w_Z_VC_SELECTOR_TO_DOWN[zWireId]);
                            router_VC->o_VC_OUT[rPortId](w_Z_VC_SELECTOR_TO_UP[zWireId]);
                        } else {
                            throw std::runtime_error("[SoCIN-3D] -- ERROR: The router instantiated is not a VC router.");
                        }
                    }
                }

                if( useDown ) {
                    zWireId =  routerId - (X_SIZE*Y_SIZE);
                    rPortId = useNorth + useEast + useSouth + useWest + useUp + useDown;
                    router_VC->i_DATA_IN[rPortId](w_Z_DATA_TO_UP[zWireId]);
                    router_VC->i_VALID_IN[rPortId](w_Z_VALID_TO_UP[zWireId]);
                    router_VC->o_RETURN_IN[rPortId](w_Z_RETURN_TO_UP[zWireId]);
                    router_VC->o_DATA_OUT[rPortId](w_Z_DATA_TO_DOWN[zWireId]);
                    router_VC->o_VALID_OUT[rPortId](w_Z_VALID_TO_DOWN[zWireId]);
                    router_VC->i_RETURN_OUT[rPortId](w_Z_RETURN_TO_DOWN[zWireId]);
                    if( NUM_VC > 1 ) {
                        if( router_VC != NULL ) {
                            router_VC->i_VC_IN[rPortId](w_Z_VC_SELECTOR_TO_UP[zWireId]);
                            router_VC->o_VC_OUT[rPortId](w_Z_VC_SELECTOR_TO_DOWN[zWireId]);
                        } else {
                            throw std::runtime_error("[SoCIN-3D] -- ERROR: The router instantiated is not a VC router.");
                        }
                    }
                }

                u_ROUTER[routerId] = router_VC;

            }
        }
    }

#ifdef WAVEFORM_SOCIN
    tf = sc_create_vcd_trace_file("socin_3d");
    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,i_RST,"RST");
    unsigned short vcBit;
    // Interfaces - routers
    for( i = 0; i < numInterfaces; i++ ) {
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
        if( NUM_VC > 1 ) {
            for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
                char strVcIn[18];
                sprintf(strVcIn,"VC_IN%s(%u)",strI,vcBit);
                sc_trace(tf,i_VC_SELECTOR[i][vcBit],strVcIn);
            }
        }

        char strDataOut[15];
        sprintf(strDataOut,"DATA_OUT%s",strI);
        sc_trace(tf, o_DATA_OUT[i],strDataOut);
        char strValidOut[15];
        sprintf(strValidOut,"VALID_OUT%s",strI);
        sc_trace(tf, o_VALID_OUT[i],strValidOut);
        char strRetOut[15];
        sprintf(strRetOut,"RET_OUT%s",strI);
        sc_trace(tf, i_RETURN_OUT[i],strRetOut);
        if( NUM_VC > 1 ) {
            for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
                char strVcOut[19];
                sprintf(strVcOut,"VC_OUT%s(%u)",strI,vcBit);
                sc_trace(tf,o_VC_SELECTOR[i][vcBit],strVcOut);
            }
        }
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

        if( NUM_VC > 1 ) {
            char wXVCLeft[30];
            sprintf(wXVCLeft,"X-VC_Sel-to-Left%s",strI);
            for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
                char wXVCLeftBit[35];
                sprintf(wXVCLeftBit,"%s(%u)",wXVCLeft,vcBit);
                sc_trace(tf,w_X_VC_SELECTOR_TO_LEFT[i][vcBit],wXVCLeftBit);
            }
        }


        char wXDataRight[21];
        sprintf(wXDataRight,"X-Data-to-Right%s",strI);
        sc_trace(tf,w_X_DATA_TO_RIGHT[i],wXDataRight);

        char wXValidRight[22];
        sprintf(wXValidRight,"X-Valid-to-Right%s",strI);
        sc_trace(tf,w_X_VALID_TO_RIGHT[i],wXValidRight);

        char wXRetRight[22];
        sprintf(wXRetRight,"X-Ret-to-Right%s",strI);
        sc_trace(tf,w_X_RETURN_TO_RIGHT[i],wXRetRight);

        if( NUM_VC > 1 ) {
            char wXVCRight[31];
            sprintf(wXVCRight,"X-VC_Sel-to-Right%s",strI);
            for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
                char wXVCRightBit[36];
                sprintf(wXVCRightBit,"%s(%u)",wXVCRight,vcBit);
                sc_trace(tf,w_X_VC_SELECTOR_TO_RIGHT[i][vcBit],wXVCRightBit);
            }
        }
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

        if( NUM_VC > 1 ) {
            char wYVCSouth[31];
            sprintf(wYVCSouth,"Y-VC_Sel-to-South%s",strI);
            for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
                char wYVCSouthBit[36];
                sprintf(wYVCSouthBit,"%s(%u)",wYVCSouth,vcBit);
                sc_trace(tf,w_Y_VC_SELECTOR_TO_SOUTH[i][vcBit],wYVCSouthBit);
            }
        }


        char wYDataNorth[21];
        sprintf(wYDataNorth,"Y-Data-to-North%s",strI);
        sc_trace(tf,w_Y_DATA_TO_NORTH[i],wYDataNorth);

        char wYValidNorth[22];
        sprintf(wYValidNorth,"Y-Valid-to-North%s",strI);
        sc_trace(tf,w_Y_VALID_TO_NORTH[i],wYValidNorth);

        char wYRetNorth[22];
        sprintf(wYRetNorth,"Y-Ret-to-North%s",strI);
        sc_trace(tf,w_Y_RETURN_TO_NORTH[i],wYRetNorth);

        if( NUM_VC > 1 ) {
            char wYVCNorth[31];
            sprintf(wYVCNorth,"Y-VC_Sel-to-North%s",strI);
            for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
                char wYVCNorthBit[36];
                sprintf(wYVCNorthBit,"%s(%u)",wYVCNorth,vcBit);
                sc_trace(tf,w_Y_VC_SELECTOR_TO_NORTH[i][vcBit],wYVCNorthBit);
            }
        }
    }
    // Z wires
    for( i = 0; i < numberOfZWires; i++) {
        char strI[5];
        sprintf(strI,"(%u)",i);

        char wZDataUp[21];
        sprintf(wZDataUp,"Z-Data-to-Up%s",strI);
        sc_trace(tf,w_Z_DATA_TO_UP[i],wZDataUp);

        char wZValidUp[22];
        sprintf(wZValidUp,"Z-Valid-to-Up%s",strI);
        sc_trace(tf,w_Z_VALID_TO_UP[i],wZValidUp);

        char wZRetUp[22];
        sprintf(wZRetUp,"Z-Ret-to-Up%s",strI);
        sc_trace(tf,w_Z_RETURN_TO_UP[i],wZRetUp);

        if( NUM_VC > 1 ) {
            char wZVCUp[31];
            sprintf(wZVCUp,"Z-VC_Sel-to-Up%s",strI);
            for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
                char wZVCUpBit[36];
                sprintf(wZVCUpBit,"%s(%u)",wZVCUp,vcBit);
                sc_trace(tf,w_Z_VC_SELECTOR_TO_UP[i][vcBit],wZVCUpBit);
            }
        }


        char wZDataDown[21];
        sprintf(wZDataDown,"Z-Data-to-Down%s",strI);
        sc_trace(tf,w_Z_DATA_TO_DOWN[i],wZDataDown);

        char wZValidDown[22];
        sprintf(wZValidDown,"Z-Valid-to-Down%s",strI);
        sc_trace(tf,w_Z_VALID_TO_DOWN[i],wZValidDown);

        char wZRetDown[22];
        sprintf(wZRetDown,"Z-Ret-to-Down%s",strI);
        sc_trace(tf,w_Z_RETURN_TO_DOWN[i],wZRetDown);

        if( NUM_VC > 1 ) {
            char wZVCDown[31];
            sprintf(wZVCDown,"Z-VC_Sel-to-Down%s",strI);
            for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
                char wZVCDownBit[36];
                sprintf(wZVCDownBit,"%s(%u)",wZVCDown,vcBit);
                sc_trace(tf,w_Z_VC_SELECTOR_TO_DOWN[i][vcBit],wZVCDownBit);
            }
        }
    }

#endif
}

SoCIN_3D::~SoCIN_3D() {
#ifdef WAVEFORM_SOCIN
    sc_close_vcd_trace_file(tf);
#endif
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
        try {
            return new SoCIN_3D(moduleName);
        } catch(const std::runtime_error& error) {
            std::cout << "Error to allocate the NoC: " << error.what() << std::endl;
            return NULL;
        }
        return NULL;

    }
    SS_EXP void delete_NoC(INoC* noc) {
        delete noc;
    }
}
