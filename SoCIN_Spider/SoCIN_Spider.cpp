#include "SoCIN_Spider.h"
#include "../Router/Router.h"
#include "../PluginManager/PluginManager.h"

//#define WAVEFORM_SOCIN

SoCIN_Spider::SoCIN_Spider(sc_module_name mn)
    : INoC_VC(mn, (X_SIZE*Y_SIZE % 2 == 0? X_SIZE*Y_SIZE:X_SIZE*Y_SIZE+1),NUM_VC),
      w_DATA_TO_LEFT("w_X_DATA_TO_LEFT"),
      w_VALID_TO_LEFT("w_X_VALID_TO_LEFT"),
      w_RETURN_TO_LEFT("w_X_RETURN_TO_LEFT"),
      w_VC_SELECTOR_TO_LEFT("w_X_VC_SELECTOR_TO_LEFT"),
      w_DATA_TO_RIGHT("w_X_DATA_TO_RIGHT"),
      w_VALID_TO_RIGHT("w_X_VALID_TO_RIGHT"),
      w_RETURN_TO_RIGHT("w_X_RETURN_TO_RIGHT"),
      w_VC_SELECTOR_TO_RIGHT("w_X_VC_SELECTOR_TO_RIGHT"),
      w_DATA_ACROSS("w_X_DATA_TO_ACROSS"),
      w_VALID_ACROSS("w_X_VALID_TO_ACROSS"),
      w_RETURN_ACROSS("w_X_RETURN_TO_ACROSS"),
      w_VC_SELECTOR_ACROSS("w_X_VC_SELECTOR_TO_ACROSS")
{
    unsigned short numberOfElements = numInterfaces;

    // Allocate the number of routers needed
    u_ROUTER.resize(numberOfElements,NULL);

    // Allocating wires
    w_DATA_TO_LEFT.init(numberOfElements);
    w_VALID_TO_LEFT.init(numberOfElements);
    w_RETURN_TO_LEFT.init(numberOfElements);
    w_DATA_TO_RIGHT.init(numberOfElements);
    w_VALID_TO_RIGHT.init(numberOfElements);
    w_RETURN_TO_RIGHT.init(numberOfElements);

    w_DATA_ACROSS.init(numberOfElements);
    w_VALID_ACROSS.init(numberOfElements);
    w_RETURN_ACROSS.init(numberOfElements);

    if( NUM_VC > 1 ) {
        w_VC_SELECTOR_TO_LEFT.init(numberOfElements);
        w_VC_SELECTOR_TO_RIGHT.init(numberOfElements);
        w_VC_SELECTOR_ACROSS.init(numberOfElements);
        for( unsigned short i = 0; i < numberOfElements; i++) {
            w_VC_SELECTOR_TO_LEFT[i].init(widthVcSelector);
            w_VC_SELECTOR_TO_RIGHT[i].init(widthVcSelector);
            w_VC_SELECTOR_ACROSS[i].init(widthVcSelector);
        }
    }

    unsigned short routerId, // Aux. to convert cartesian coordinate to number router id
        rPortId,        // Aux. to identify the port id to be binded in connections between routers
        wireId,         // Aux. to identify the wires id to binding connections between routers
        wIdToAcross,    // Aux. to identify the wire id to binding output signal in across router connection
        wIdFromAcross;  // Aux. to identify the wire id to binding input signal in across router connection

    for( routerId = 0; routerId < numberOfElements; routerId++ ) {
        char rName[15];
        sprintf(rName,"ParIS[%u]",routerId);
        unsigned short x,y;
        x = ID_TO_COORDINATE_X(routerId);
        y = ID_TO_COORDINATE_Y(routerId);

        // Instantiating a router
        IRouter* router = PLUGIN_MANAGER->routerInstance(rName,x,y,4,NUM_VC);
        if(router == NULL) {
            std::cout << "\n\t[SoCIN_Spider] -- ERROR: It was not possible instantiate a router." << std::endl;
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
                std::cout << "\n\t[SoCIN_Spider] -- ERROR: The router instantiated is not a VC router." << std::endl;
                return;
            }
        }

        // Port 1 is LEFT port
        rPortId = 1;
        wireId = routerId;
        router->i_DATA_IN[rPortId](w_DATA_TO_RIGHT[wireId]);
        router->i_VALID_IN[rPortId](w_VALID_TO_RIGHT[wireId]);
        router->o_RETURN_IN[rPortId](w_RETURN_TO_RIGHT[wireId]);
        router->o_DATA_OUT[rPortId](w_DATA_TO_LEFT[wireId]);
        router->o_VALID_OUT[rPortId](w_VALID_TO_LEFT[wireId]);
        router->i_RETURN_OUT[rPortId](w_RETURN_TO_LEFT[wireId]);
        if( NUM_VC > 1 ) {
            if( router_VC != NULL ) { // Once a time is needed this test, so here it is not needed because already verified in external interface.
                router_VC->i_VC_IN[rPortId](w_VC_SELECTOR_TO_RIGHT[wireId]);
                router_VC->o_VC_OUT[rPortId](w_VC_SELECTOR_TO_LEFT[wireId]);
            } else {
                std::cout << "\n\t[SoCIN_Spider] -- ERROR: The router instantiated is not a VC router." << std::endl;
                return;
            }
        }

        // Port 2 is the RIGHT port
        rPortId = 2;
        wireId = routerId == 0 ? numInterfaces-1 : routerId - 1;
        router->i_DATA_IN[rPortId](w_DATA_TO_LEFT[wireId]);
        router->i_VALID_IN[rPortId](w_VALID_TO_LEFT[wireId]);
        router->o_RETURN_IN[rPortId](w_RETURN_TO_LEFT[wireId]);
        router->o_DATA_OUT[rPortId](w_DATA_TO_RIGHT[wireId]);
        router->o_VALID_OUT[rPortId](w_VALID_TO_RIGHT[wireId]);
        router->i_RETURN_OUT[rPortId](w_RETURN_TO_RIGHT[wireId]);
        if( NUM_VC > 1 ) {
            if( router_VC != NULL ) {
                router_VC->i_VC_IN[rPortId](w_VC_SELECTOR_TO_LEFT[wireId]);
                router_VC->o_VC_OUT[rPortId](w_VC_SELECTOR_TO_RIGHT[wireId]);
            } else {
                std::cout << "\n\t[SoCIN_Spider] -- ERROR: The router instantiated is not a VC router." << std::endl;
                return;
            }
        }

        // Port 3 is the ACROSS port
        rPortId = 3;
        wIdFromAcross = routerId < numberOfElements/2 ? numberOfElements/2 + routerId : routerId - numberOfElements/2;
        wIdToAcross = routerId;
        router->i_DATA_IN[rPortId](w_DATA_ACROSS[wIdFromAcross]);
        router->i_VALID_IN[rPortId](w_VALID_ACROSS[wIdFromAcross]);
        router->o_RETURN_IN[rPortId](w_RETURN_ACROSS[wIdFromAcross]);
        router->o_DATA_OUT[rPortId](w_DATA_ACROSS[wIdToAcross]);
        router->o_VALID_OUT[rPortId](w_VALID_ACROSS[wIdToAcross]);
        router->i_RETURN_OUT[rPortId](w_RETURN_ACROSS[wIdToAcross]);
        if( NUM_VC > 1 ) {
            if( router_VC != NULL ) {
                router_VC->i_VC_IN[rPortId](w_VC_SELECTOR_ACROSS[wIdFromAcross]);
                router_VC->o_VC_OUT[rPortId](w_VC_SELECTOR_ACROSS[wIdToAcross]);
            } else {
                std::cout << "\n\t[SoCIN_Spider] -- ERROR: The router instantiated is not a VC router." << std::endl;
                return;
            }
        }

        u_ROUTER[routerId] = router;
    }
#ifdef WAVEFORM_SOCIN
    tf = sc_create_vcd_trace_file("socin_spider");
    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,i_RST,"RST");
    unsigned short i,vcBit;
    // Interfaces - routers
    for( i = 0; i < numberOfElements; i++ ) {
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

    // Wires
    for( i = 0; i < numberOfElements; i++) {
        char strI[5];
        sprintf(strI,"(%u)",i);

        char wDataLeft[20];
        sprintf(wDataLeft,"Data-to-Left%s",strI);
        sc_trace(tf,w_DATA_TO_LEFT[i],wDataLeft);

        char wValidLeft[21];
        sprintf(wValidLeft,"Valid-to-Left%s",strI);
        sc_trace(tf,w_VALID_TO_LEFT[i],wValidLeft);

        char wRetLeft[21];
        sprintf(wRetLeft,"Ret-to-Left%s",strI);
        sc_trace(tf,w_RETURN_TO_LEFT[i],wRetLeft);

        char wVCLeft[30];
        sprintf(wVCLeft,"VC_Sel-to-Left%s",strI);
        for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
            char wVCLeftBit[35];
            sprintf(wVCLeftBit,"%s(%u)",wVCLeft,vcBit);
            sc_trace(tf,w_VC_SELECTOR_TO_LEFT[i][vcBit],wVCLeftBit);
        }


        char wDataRight[21];
        sprintf(wDataRight,"Data-to-Right%s",strI);
        sc_trace(tf,w_DATA_TO_RIGHT[i],wDataRight);

        char wValidRight[22];
        sprintf(wValidRight,"Valid-to-Right%s",strI);
        sc_trace(tf,w_VALID_TO_RIGHT[i],wValidRight);

        char wRetRight[22];
        sprintf(wRetRight,"Ret-to-Right%s",strI);
        sc_trace(tf,w_RETURN_TO_RIGHT[i],wRetRight);

        char wVCRight[31];
        sprintf(wVCRight,"VC_Sel-to-Right%s",strI);
        for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
            char wVCRightBit[36];
            sprintf(wVCRightBit,"%s(%u)",wVCRight,vcBit);
            sc_trace(tf,w_VC_SELECTOR_TO_RIGHT[i][vcBit],wVCRightBit);
        }


        char wDataAcross[22];
        sprintf(wDataAcross,"Data-Across%s",strI);
        sc_trace(tf,w_DATA_ACROSS[i],wDataAcross);

        char wValidAcross[23];
        sprintf(wValidAcross,"Valid-Across%s",strI);
        sc_trace(tf,w_VALID_ACROSS[i],wValidAcross);

        char wRetAcross[23];
        sprintf(wRetAcross,"Ret-Across%s",strI);
        sc_trace(tf,w_RETURN_ACROSS[i],wRetAcross);

        char wVCAcross[32];
        sprintf(wVCAcross,"VC_Sel-Across%s",strI);
        for(vcBit = 0; vcBit < widthVcSelector; vcBit++) {
            char wVCAcrossBit[37];
            sprintf(wVCAcrossBit,"%s(%u)",wVCAcross,vcBit);
            sc_trace(tf,w_VC_SELECTOR_ACROSS[i][vcBit],wVCAcrossBit);
        }
    }
#endif
}


SoCIN_Spider::~SoCIN_Spider() {
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
        return new SoCIN_Spider(moduleName);
    }
    SS_EXP void delete_NoC(INoC* noc) {
        delete noc;
    }
}
