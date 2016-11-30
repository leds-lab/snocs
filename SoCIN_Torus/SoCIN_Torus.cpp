#include "SoCIN_Torus.h"
#include "../Router/Router.h"
#include "../PluginManager/PluginManager.h"

//#define WAVEFORM_SOCIN

SoCIN_Torus::SoCIN_Torus(sc_module_name mn)
    : INoC_VC(mn, (X_SIZE*Y_SIZE), NUM_VC ),
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
      w_Y_VC_SELECTOR_TO_NORTH("w_Y_VC_SELECTOR_TO_NORTH")
{
    IRouting* tester = PLUGIN_MANAGER->routingInstance("Tester",0,5);
    if(tester != NULL) {
        if( tester->supportedTopology() != this->topologyType() ) {
            throw std::runtime_error("[SoCIN_Torus] Routing incompatible with the topology");
        }
    }
    // Allocating the number of routers needed
    u_ROUTER.resize(numInterfaces,NULL);

    unsigned short numberOfXWires = numInterfaces;
    unsigned short numberOfYWires = numInterfaces;

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
        for(unsigned short i = 0; i < numberOfXWires; i++) {
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
        for(unsigned short i = 0; i < numberOfYWires; i++) {
            w_Y_VC_SELECTOR_TO_SOUTH[i].init(widthVcSelector);
            w_Y_VC_SELECTOR_TO_NORTH[i].init(widthVcSelector);
        }
    }

    unsigned short x,y, // Aux. to identify the router id in cartesian coordinates
        routerId,       // Aux. to convert cartesian coordinate to number router id
        rPortId,        // Aux. to identify the port id to be binded in connections between routers
        xWireId,yWireId;// Aux. to identify the wires id to binding connections between routers

    for( x = 0; x < X_SIZE; x++ ) {
        for( y = 0; y < Y_SIZE; y++ ) {

            routerId = COORDINATE_2D_TO_ID(x,y);

            char rName[15];
            sprintf(rName,"ParIS[%u][%u]",x,y);

            // Instantiating a router
            IRouter* router = PLUGIN_MANAGER->routerInstance(rName,routerId,5,NUM_VC);
            if(router == NULL) {
                throw std::runtime_error("[SoCIN_Torus] -- ERROR: It was not possible instantiate a router.");
                return;
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
                    throw std::runtime_error("[SoCIN_Torus] -- ERROR: The router instantiated is not a VC router.");
                }
            }

            // Connections with adjacent/neighbour routers
            // Port 1 is NORTH port
            rPortId = 1;
            yWireId = routerId;
            router->i_DATA_IN[rPortId](w_Y_DATA_TO_SOUTH[yWireId]);
            router->i_VALID_IN[rPortId](w_Y_VALID_TO_SOUTH[yWireId]);
            router->o_RETURN_IN[rPortId](w_Y_RETURN_TO_SOUTH[yWireId]);
            router->o_DATA_OUT[rPortId](w_Y_DATA_TO_NORTH[yWireId]);
            router->o_VALID_OUT[rPortId](w_Y_VALID_TO_NORTH[yWireId]);
            router->i_RETURN_OUT[rPortId](w_Y_RETURN_TO_NORTH[yWireId]);
            if( NUM_VC > 1 ) {
                if( router_VC != NULL ) {
                    router_VC->i_VC_IN[rPortId](w_Y_VC_SELECTOR_TO_SOUTH[yWireId]);
                    router_VC->o_VC_OUT[rPortId](w_Y_VC_SELECTOR_TO_NORTH[yWireId]);
                } else {
                    throw std::runtime_error("[SoCIN_Torus] -- ERROR: The router instantiated is not a VC router.");
                }
            }

            // Port 2 is EAST port
            rPortId = 2;
            xWireId = routerId;
            router->i_DATA_IN[rPortId](w_X_DATA_TO_LEFT[xWireId]);
            router->i_VALID_IN[rPortId](w_X_VALID_TO_LEFT[xWireId]);
            router->o_RETURN_IN[rPortId](w_X_RETURN_TO_LEFT[xWireId]);
            router->o_DATA_OUT[rPortId](w_X_DATA_TO_RIGHT[xWireId]);
            router->o_VALID_OUT[rPortId](w_X_VALID_TO_RIGHT[xWireId]);
            router->i_RETURN_OUT[rPortId](w_X_RETURN_TO_RIGHT[xWireId]);
            if( NUM_VC > 1 ) {
                if( router_VC != NULL ) {
                    router_VC->i_VC_IN[rPortId](w_X_VC_SELECTOR_TO_LEFT[xWireId]);
                    router_VC->o_VC_OUT[rPortId](w_X_VC_SELECTOR_TO_RIGHT[xWireId]);
                } else {
                    throw std::runtime_error("[SoCIN_Torus] -- ERROR: The router instantiated is not a VC router.");
                }
            }

            // Port 3 is SOUTH port
            rPortId = 3;
            yWireId = y == 0 ? (X_SIZE*Y_SIZE)-X_SIZE+x : routerId-X_SIZE;
            router->i_DATA_IN[rPortId](w_Y_DATA_TO_NORTH[yWireId]);
            router->i_VALID_IN[rPortId](w_Y_VALID_TO_NORTH[yWireId]);
            router->o_RETURN_IN[rPortId](w_Y_RETURN_TO_NORTH[yWireId]);
            router->o_DATA_OUT[rPortId](w_Y_DATA_TO_SOUTH[yWireId]);
            router->o_VALID_OUT[rPortId](w_Y_VALID_TO_SOUTH[yWireId]);
            router->i_RETURN_OUT[rPortId](w_Y_RETURN_TO_SOUTH[yWireId]);
            if( NUM_VC > 1 ) {
                if( router_VC != NULL ) {
                    router_VC->i_VC_IN[rPortId](w_Y_VC_SELECTOR_TO_NORTH[yWireId]);
                    router_VC->o_VC_OUT[rPortId](w_Y_VC_SELECTOR_TO_SOUTH[yWireId]);
                } else {
                    throw std::runtime_error("[SoCIN_Torus] -- ERROR: The router instantiated is not a VC router.");
                }
            }

            // Port 4 is WEST port
            rPortId = 4;
            xWireId =  x == 0 ? routerId + X_SIZE - 1 : routerId - 1;
            router->i_DATA_IN[rPortId](w_X_DATA_TO_RIGHT[xWireId]);
            router->i_VALID_IN[rPortId](w_X_VALID_TO_RIGHT[xWireId]);
            router->o_RETURN_IN[rPortId](w_X_RETURN_TO_RIGHT[xWireId]);
            router->o_DATA_OUT[rPortId](w_X_DATA_TO_LEFT[xWireId]);
            router->o_VALID_OUT[rPortId](w_X_VALID_TO_LEFT[xWireId]);
            router->i_RETURN_OUT[rPortId](w_X_RETURN_TO_LEFT[xWireId]);
            if( NUM_VC > 1 ) {
                if( router_VC != NULL ) {
                    router_VC->i_VC_IN[rPortId](w_X_VC_SELECTOR_TO_RIGHT[xWireId]);
                    router_VC->o_VC_OUT[rPortId](w_X_VC_SELECTOR_TO_LEFT[xWireId]);
                } else {
                    throw std::runtime_error("[SoCIN_Torus] -- ERROR: The router instantiated is not a VC router.");
                }
            }

            u_ROUTER[routerId] = router;
        }
    }

#ifdef WAVEFORM_SOCIN
    tf = sc_create_vcd_trace_file("socin_torus");
    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,i_RST,"RST");
    unsigned short i,vcBit;
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
        for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
            char strVcIn[18];
            sprintf(strVcIn,"VC_IN%s(%u)",strI,vcBit);
            sc_trace(tf,i_VC_SELECTOR[i][vcBit],strVcIn);
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
        for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
            char strVcOut[19];
            sprintf(strVcOut,"VC_OUT%s(%u)",strI,vcBit);
            sc_trace(tf,o_VC_SELECTOR[i][vcBit],strVcOut);
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

        char wXVCLeft[30];
        sprintf(wXVCLeft,"X-VC_Sel-to-Left%s",strI);
        for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
            char wXVCLeftBit[35];
            sprintf(wXVCLeftBit,"%s(%u)",wXVCLeft,vcBit);
            sc_trace(tf,w_X_VC_SELECTOR_TO_LEFT[i][vcBit],wXVCLeftBit);
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

        char wXVCRight[31];
        sprintf(wXVCRight,"X-VC_Sel-to-Right%s",strI);
        for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
            char wXVCRightBit[36];
            sprintf(wXVCRightBit,"%s(%u)",wXVCRight,vcBit);
            sc_trace(tf,w_X_VC_SELECTOR_TO_RIGHT[i][vcBit],wXVCRightBit);
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

        char wYVCSouth[31];
        sprintf(wYVCSouth,"Y-VC_Sel-to-South%s",strI);
        for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
            char wYVCSouthBit[36];
            sprintf(wYVCSouthBit,"%s(%u)",wYVCSouth,vcBit);
            sc_trace(tf,w_Y_VC_SELECTOR_TO_SOUTH[i][vcBit],wYVCSouthBit);
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

        char wYVCNorth[31];
        sprintf(wYVCNorth,"Y-VC_Sel-to-North%s",strI);
        for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
            char wYVCNorthBit[36];
            sprintf(wYVCNorthBit,"%s(%u)",wYVCNorth,vcBit);
            sc_trace(tf,w_Y_VC_SELECTOR_TO_NORTH[i][vcBit],wYVCNorthBit);
        }
    }

#endif
}

SoCIN_Torus::~SoCIN_Torus() {
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
            return new SoCIN_Torus(moduleName);
        } catch(const std::runtime_error& error) {
            std::cout << "Error to allocate the NoC: " << error.what() << std::endl;
            return NULL;
        }
    }
    SS_EXP void delete_NoC(INoC* noc) {
        delete noc;
    }
}
