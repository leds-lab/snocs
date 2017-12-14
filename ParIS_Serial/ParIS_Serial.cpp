#include "ParIS_Serial.h"
#include "../src/PluginManager.h"

//#define WAVEFORM_PARIS
//#define DEBUG_PARIS

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// ParIS with N virtual channels /////////////////////////////
//////////////////////////////// and "serial" links ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
ParIS_N_VC_Serial::ParIS_N_VC_Serial(sc_module_name mn,
             unsigned short nPorts,
             unsigned short nVirtualChannels,
             unsigned short ROUTER_ID)
    : IRouter_VC(mn,nPorts,nVirtualChannels,ROUTER_ID),
      w_REQUEST("ParIS_N_VC_Serial_wREQUEST",nVirtualChannels),
      w_GRANT("ParIS_N_VC_Serial_wGRANT",nVirtualChannels),
      w_READ_OK("ParIS_N_VC_Serial_wREAD_OK",nVirtualChannels),
      w_READ("ParIS_N_VC_Serial_wREAD",nVirtualChannels),
      w_IDLE("ParIS_N_VC_Serial_wIDLE",nVirtualChannels),
      w_DATA("ParIS_N_VC_Serial_wDATA",nVirtualChannels),
      w_GND("ParIS_N_VC_Serial_wGND"),
      u_XIN(nPorts,NULL),
      u_XOUT(nPorts,NULL),
      u_CONVERTER(nPorts,NULL),
      w_VALID("ParIS_N_VC_Serial_wVALID",nPorts),
      w_RETURN("ParIS_N_VC_Serial_wRETURN",nPorts),
      w_DATA_CONV("ParIS_N_VC_Serial_wDATA_CONV",nPorts)
{
    unsigned short i,o,vc;

    // Configuring channels according the number of ports and virtual channels
    for( vc = 0; vc < nVirtualChannels; vc++ ) {
        w_REQUEST[vc].init(nPorts);
        w_GRANT[vc].init(nPorts);
        w_READ_OK[vc].init(nPorts);
        w_READ[vc].init(nPorts);
        w_IDLE[vc].init(nPorts);
        w_DATA[vc].init(nPorts);
        for(i = 0; i < nPorts; i++) {
            w_REQUEST[vc][i].init(nPorts);
            w_GRANT[vc][i].init(nPorts);
        }
    }

    // Instantiating internal modules
    for( i = 0; i < nPorts; i++) {
        // Assembling internal module names
        // XIN(i)
        char strXIN[12];
        sprintf(strXIN,"XIN(%u)",i);
        // XOUT(i)
        char strXOUT[13];
        sprintf(strXOUT,"XOUT(%u)",i);

        // Converter(i)
        char strConverter[10];
        sprintf(strConverter,"Conv(%u)",i);

        // Instantiate internal units
        u_XIN[i] = new XIN_N_VC(strXIN,nPorts,nVirtualChannels,ROUTER_ID,i);
        u_XOUT[i] = new XOUT_N_VC(strXOUT,nPorts,nVirtualChannels,ROUTER_ID,i);
        u_CONVERTER[i] = new Converter(strConverter,ROUTER_ID,i);
    }

    // Binding ports
    for( i = 0; i < nPorts; i++ ) {
        XIN_N_VC* xin = u_XIN[i];
        // Binding XIN
        // System signals
        xin->i_CLK(i_CLK);
        xin->i_RST(i_RST);
        // Link signals
        xin->i_DATA(i_DATA_IN[i]);
        xin->i_VALID(i_VALID_IN[i]);
        xin->o_RETURN(o_RETURN_IN[i]);

        xin->i_VC_SELECTOR(i_VC_IN[i]);
        // Inter-module's signals
        for( vc = 0; vc < nVirtualChannels; vc++) {
            for( o = 0; o < nPorts; o++ ) {
                xin->o_X_REQUEST[vc][o](w_REQUEST[vc][i][o]);
                xin->i_X_GRANT[vc][o](w_GRANT[vc][o][i]);
            }
            xin->o_X_READ_OK[vc](w_READ_OK[vc][i]);
            xin->i_X_READ[vc](w_READ[vc]);
            xin->i_X_IDLE[vc](w_IDLE[vc]);
            xin->o_X_DATA[vc](w_DATA[vc][i]);
        }

        XOUT_N_VC* xout = u_XOUT[i];
        Converter* conv = u_CONVERTER[i];
        // Bindig XOUT and converter
        // System signals
        xout->i_CLK(i_CLK);
        xout->i_RST(i_RST);
        conv->i_CLK(i_CLK);
        conv->i_RST(i_RST);
        // Link signals
        conv->i_VALID(w_VALID[i]);
        conv->i_RETURN(i_RETURN_OUT[i]);
        conv->i_DATA(w_DATA_CONV[i]);
        conv->o_VALID(o_VALID_OUT[i]);
        conv->o_RETURN(w_RETURN[i]);
        conv->o_DATA(o_DATA_OUT[i]);

        xout->o_VALID(w_VALID[i]);
        xout->i_RETURN(w_RETURN[i]);
        xout->o_DATA(w_DATA_CONV[i]);
//        xout->o_VC_SELECTOR();
        xout->o_VC_SELECTOR(o_VC_OUT[i]);

//        xout->o_VALID(o_VALID_OUT[i]);
//        xout->i_RETURN(i_RETURN_OUT[i]);
//        xout->o_DATA(o_DATA_OUT[i]);

        // Inter-module's signals
        for( vc = 0; vc < nVirtualChannels; vc++) {
            xout->i_HOLD_SEND[vc](w_GND);
            for( o = 0; o < nPorts; o++) {
                xout->i_X_REQUEST[vc][o](w_REQUEST[vc][o][i]);
                xout->o_X_GRANT[vc][o](w_GRANT[vc][i][o]);
            }
            xout->i_X_READ_OK[vc](w_READ_OK[vc]);
            xout->o_X_IDLE[vc](w_IDLE[vc][i]);
            xout->o_X_READ[vc](w_READ[vc][i]);
            xout->i_X_DATA[vc](w_DATA[vc]);
        }
    }

    // Registering process
    SC_METHOD(p_GND);
    sensitive << i_RST;

#ifdef DEBUG_PARIS
    SC_METHOD(p_DEBUG);
    sensitive << i_CLK.pos();
#endif
#ifdef WAVEFORM_PARIS
    char waveName[20];
    sprintf(waveName,"ParIS_Serial_%u_VC%u_Port-%u",ROUTER_ID,numVirtualChannels,nPorts);
    tf = sc_create_vcd_trace_file(waveName);

    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,i_RST,"RST");

    for( i = 0; i < nPorts; i++ ) {
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
    for( vc = 0; vc < nVirtualChannels; vc++) {
        for( i = 0; i < nPorts; i++) {
            char strIVc[20];
            sprintf(strIVc,"Port(%u)Vc(%u)",i,vc);
            for( o = 0; o < nPorts; o++ ) {
                char strIVcJ[30];
                sprintf(strIVcJ,"%s(%u)",strIVc,o);

                char strReq[36];
                sprintf(strReq,"wREQ%s",strIVcJ);
                sc_trace(tf,w_REQUEST[vc][i][o],strReq);

                char strGrant[38];
                sprintf(strGrant,"wGRANT%s",strIVcJ);
                sc_trace(tf,w_GRANT[vc][i][o],strGrant);
            }

            char strRok[30];
            sprintf(strRok,"wROK%s",strIVc);
            sc_trace(tf,w_READ_OK[vc][i],strRok);

            char strRd[29];
            sprintf(strRd,"wRD%s",strIVc);
            sc_trace(tf,w_READ[vc][i],strRd);

            char strIdle[31];
            sprintf(strIdle,"wIDLE%s",strIVc);
            sc_trace(tf,w_IDLE[vc][i],strIdle);

            char strData[31];
            sprintf(strData,"wDATA%s",strIVc);
            sc_trace(tf,w_DATA[vc][i],strData);
        }
    }

#endif
}

ParIS_N_VC_Serial::~ParIS_N_VC_Serial() {
#ifdef WAVEFORM_PARIS
    sc_close_vcd_trace_file(tf);
#endif
    for(unsigned short i = 0; i < numPorts; i++) {
        XIN_N_VC* xin = u_XIN[i];
        if(xin) {
            delete xin;
        }
        XOUT_N_VC* xout = u_XOUT[i];
        if(xout) {
            delete xout;
        }
        Converter* conv = u_CONVERTER[i];
        if(conv) {
            delete conv;
        }
    }
    u_XIN.clear();
    u_XOUT.clear();
    u_CONVERTER.clear();
}

void ParIS_N_VC_Serial::p_DEBUG() {

    std::string str = sc_time_stamp().to_string();

    for(unsigned short v = 0; v < numVirtualChannels; v++) {
        for( unsigned short i = 0; i < numPorts; i++ ) {
            for( unsigned short x = 0; x < numPorts; x++ ) {
                bool req = w_REQUEST[v][i][x].read();
                bool grant = w_GRANT[v][i][x].read();
                if(req) {
                    printf("\n[Router] [%u][%u] - time: %s, Req[%u][%u]",ROUTER_ID,v,str.c_str(),i,x);
                }
                if(grant) {
                    printf("\n[Router] [%u][%u] - time: %s, Grant[%u][%u]",ROUTER_ID,v,str.c_str(),i,x);
                }

            }
        }
    }

    for(unsigned short v = 0; v < numVirtualChannels; v++) {
        for( unsigned short i = 0; i < numPorts; i++ ) {
            if( ROUTER_ID == 0 ) {
                Flit d = w_DATA[v][i].read();
                printf("\n[ParIS_Serial] VC[%u] PORT[%u] IDLE: %d- RD: %d- ROK: %d- DATA: %s",
                       v,i,w_IDLE[v][i].read(),w_READ[v][i].read(),w_READ_OK[v][i].read(),
                       d.data.to_string(SC_HEX_US).c_str());
            }
        }
    }
}



///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// ParIS from SoCINfp //////////////////////////////////
///////////////////////////////// and "serial" links //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
ParIS_Serial::ParIS_Serial(sc_module_name mn,
             unsigned short nPorts,
             unsigned short ROUTER_ID)
    : IRouter(mn,nPorts,ROUTER_ID),
      w_REQUEST("ParIS_Serial_wREQUEST",nPorts),
      w_GRANT("ParIS_Serial_wGRANT",nPorts),
      w_READ_OK("ParIS_Serial_wREAD_OK",nPorts),
      w_READ("ParIS_Serial_wREAD",nPorts),
      w_IDLE("ParIS_Serial_wIDLE",nPorts),
      w_DATA("ParIS_Serial_wDATA",nPorts),
      w_GND("ParIS_Serial_wGND"),
      u_XIN(nPorts,NULL),
      u_XOUT(nPorts,NULL),
      u_CONVERTER(nPorts,NULL),
      w_VALID("ParIS_Serial_wVALID",nPorts),
      w_RETURN("ParIS_Serial_wRETURN",nPorts),
      w_DATA_CONV("ParIS_Serial_wDATA_CONV",nPorts)
{
    unsigned short i,j;
    for( i = 0; i < nPorts; i++ ) {
        w_REQUEST[i].init(nPorts);
        w_GRANT[i].init(nPorts);
    }

    // Instantiating internal modules
    for( i = 0; i < nPorts; i++) {
        // Assembling internal module names
        // XIN(i)
        char strXIN[12];
        sprintf(strXIN,"XIN(%u)",i);
        // XOUT(i)
        char strXOUT[13];
        sprintf(strXOUT,"XOUT(%u)",i);

        // Getting input units
        char strMemIn[20];
        sprintf(strMemIn,"%s_MEM_IN",strXIN);
        IMemory*  u_i_MEM = PLUGIN_MANAGER->memoryInstance(strMemIn,ROUTER_ID,i,FIFO_IN_DEPTH);
        char strRouting[30];
        sprintf(strRouting,"%s_ROUTING",strXIN);
        IRouting* u_ROUTING = PLUGIN_MANAGER->routingInstance(strRouting,ROUTER_ID,i,nPorts);
        char strIFC[20];
        sprintf(strIFC,"%s_IFC",strXIN);
        IInputFlowControl* u_IFC = PLUGIN_MANAGER->inputFlowControlInstance(strIFC,ROUTER_ID,i);

        // Getting output units
        char strMemOut[20];
        sprintf(strMemOut,"%s_MEM_OUT",strXOUT);
        IMemory*  u_o_MEM = PLUGIN_MANAGER->memoryInstance(strMemOut,ROUTER_ID,i,FIFO_OUT_DEPTH);
        char strPG[20];
        sprintf(strPG,"%s_PG",strXOUT);
        IPriorityGenerator* u_PG = PLUGIN_MANAGER->priorityGeneratorInstance(strPG,ROUTER_ID,i,nPorts);
        char strArb[35];
        sprintf(strArb,"%s_DIST_ARBITER",strXOUT);
        IArbiter* u_ARBITER = new DistributedArbiter(strArb,nPorts,u_PG,ROUTER_ID,i);
        char strOFC[20];
        sprintf(strOFC,"%s_OFC",strXOUT);
        // The last argument is used only in credit-based flow control
        IOutputFlowControl* u_OFC = PLUGIN_MANAGER->outputFlowControlInstance(strOFC,ROUTER_ID,i,CREDIT);

        // Instantiate internal units
        u_XIN[i] = new XIN_none_VC(strXIN,u_i_MEM,u_ROUTING,u_IFC,nPorts,ROUTER_ID,i);
        u_XOUT[i] = new XOUT_none_VC(strXOUT,u_o_MEM,u_ARBITER,u_OFC,nPorts,ROUTER_ID,i);

        char strConverter[10];
        sprintf(strConverter,"conv(%u)",i);
        u_CONVERTER[i] = new Converter(strConverter,ROUTER_ID,i);
    }

    // Binding ports
    for( i = 0; i < nPorts; i++ ) {
        XIN_none_VC* xin = u_XIN[i];
        // Binding XIN
        // System signals
        xin->i_CLK(i_CLK);
        xin->i_RST(i_RST);
        // Link signals
        xin->i_DATA(i_DATA_IN[i]);
        xin->i_VALID(i_VALID_IN[i]);
        xin->o_RETURN(o_RETURN_IN[i]);
        // Inter-module's signals
        for( j = 0; j < nPorts; j++ ) {
            xin->o_X_REQUEST[j](w_REQUEST[i][j]);
            xin->i_X_GRANT[j](w_GRANT[j][i]);
        }
        xin->o_X_READ_OK(w_READ_OK[i]);
        xin->i_X_READ(w_READ);
        xin->i_X_IDLE(w_IDLE);
        xin->o_X_DATA(w_DATA[i]);

        XOUT_none_VC* xout = u_XOUT[i];
        Converter* conv = u_CONVERTER[i];
        // Bindig XOUT and converter
        // System signals
        xout->i_CLK(i_CLK);
        xout->i_RST(i_RST);
        conv->i_CLK(i_CLK);
        conv->i_RST(i_RST);
        // Link signals
        conv->i_VALID(w_VALID[i]);
        conv->i_RETURN(i_RETURN_OUT[i]);
        conv->i_DATA(w_DATA_CONV[i]);
        conv->o_VALID(o_VALID_OUT[i]);
        conv->o_RETURN(w_RETURN[i]);
        conv->o_DATA(o_DATA_OUT[i]);

        xout->o_VALID(w_VALID[i]);
        xout->i_RETURN(w_RETURN[i]);
        xout->o_DATA(w_DATA_CONV[i]);

//        xout->o_VALID(o_VALID_OUT[i]);
//        xout->i_RETURN(i_RETURN_OUT[i]);
//        xout->o_DATA(o_DATA_OUT[i]);

        // Link signals
//        xout->o_VALID(o_VALID_OUT[i]);
//        xout->i_RETURN(i_RETURN_OUT[i]);
//        xout->o_DATA(o_DATA_OUT[i]);
        xout->i_HOLD_SEND(w_GND);

        // Inter-module's signals
        for( j = 0; j < nPorts; j++) {
            xout->i_X_REQUEST[j](w_REQUEST[j][i]);
            xout->o_X_GRANT[j](w_GRANT[i][j]);
        }
        xout->i_X_READ_OK(w_READ_OK);
        xout->o_X_IDLE(w_IDLE[i]);
        xout->o_X_READ(w_READ[i]);
        xout->i_X_DATA(w_DATA);
    }

    // Registering process
    SC_METHOD(p_GND);
    sensitive << i_RST;

#ifdef DEBUG_PARIS
    SC_METHOD(p_DEBUG);
    sensitive << i_CLK.pos();
#endif
#ifdef WAVEFORM_PARIS
    char waveName[20];
    sprintf(waveName,"ParIS_Serial_%u_%u",ROUTER_ID,nPorts);
    tf = sc_create_vcd_trace_file(waveName);

    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,i_RST,"RST");

    for( i = 0; i < nPorts; i++ ) {
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

        for( j = 0; j < nPorts; j++ ) {
            char strIJ[10];
            sprintf(strIJ,"%s(%u)",strI,j);

            char strReq[16];
            sprintf(strReq,"wREQ%s",strIJ);
            sc_trace(tf,w_REQUEST[i][j],strReq);

            char strGrant[18];
            sprintf(strGrant,"wGRANT%s",strIJ);
            sc_trace(tf,w_GRANT[i][j],strGrant);
        }

        char strRok[10];
        sprintf(strRok,"wROK%s",strI);
        sc_trace(tf,w_READ_OK[i],strRok);

        char strRd[9];
        sprintf(strRd,"wRD%s",strI);
        sc_trace(tf,w_READ[i],strRd);

        char strIdle[11];
        sprintf(strIdle,"wIDLE%s",strI);
        sc_trace(tf,w_IDLE[i],strIdle);

        char strData[11];
        sprintf(strData,"wDATA%s",strI);
        sc_trace(tf,w_DATA[i],strData);

        char strDataConv[20];
        sprintf(strDataConv,"wDATA_Conv%s",strI);
        sc_trace(tf,w_DATA_CONV[i],strDataConv);

    }

#endif
}

ParIS_Serial::~ParIS_Serial() {
#ifdef WAVEFORM_PARIS
    sc_close_vcd_trace_file(tf);
#endif
    for(unsigned short i = 0; i < numPorts; i++) {
        XIN_none_VC* xin = u_XIN[i];
        if(xin) {
            delete xin;
        }
        XOUT_none_VC* xout = u_XOUT[i];
        if(xout) {
            delete xout;
        }
        Converter* conv = u_CONVERTER[i];
        if(conv) {
            delete conv;
        }
    }
    u_XIN.clear();
    u_XOUT.clear();
    u_CONVERTER.clear();
}

void ParIS_Serial::p_DEBUG() {

    std::string str = sc_time_stamp().to_string();
/*
    for( unsigned short i = 0; i < numPorts; i++ ) {
        for( unsigned short x = 0; x < numPorts; x++ ) {
            bool req = w_REQUEST[i][x].read();
            bool grant = w_GRANT[i][x].read();
            if(req) {
                printf("\n[Router][%u] - time: %s, Req[%u][%u]",ROUTER_ID,str.c_str(),i,x);
            }
            if(grant) {
                printf("\n[Router][%u] - time: %s, Grant[%u][%u]",ROUTER_ID,str.c_str(),i,x);
            }

        }
    }
*/
    for( unsigned short i = 0; i < numPorts; i++ ) {
        Flit d = w_DATA[i].read();
        printf("\n[ParIS_Serial][%u] PORT[%u] IDLE: %d- RD: %d- ROK: %d- DATA: %s",ROUTER_ID,
               i,w_IDLE[i].read(),w_READ[i].read(),w_READ_OK[i].read(),d.data.to_string(SC_HEX_US).c_str());
    }

}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Factory Methods Routers ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
extern "C" {
    SS_EXP IRouter* new_Router(sc_simcontext* simcontext,
                               sc_module_name moduleName,
                               unsigned short int nPorts,
                               unsigned short int nVirtualChannels,
                               unsigned short int ROUTER_ID) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        if( nVirtualChannels > 1 ) {
            return new ParIS_N_VC_Serial(moduleName,nPorts,nVirtualChannels,ROUTER_ID);
        } else {
            return new ParIS_Serial(moduleName,nPorts,ROUTER_ID);
        }

    }
    SS_EXP void delete_Router(IRouter* router) {
        delete router;
    }
}
