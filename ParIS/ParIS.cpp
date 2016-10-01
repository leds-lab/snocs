#include "ParIS.h"
#include "../PluginManager/PluginManager.h"

ParIS::ParIS(sc_module_name mn,
             unsigned short nPorts,
             unsigned short XID,
             unsigned short YID)
    : IRouter(mn,nPorts,XID,YID),
      w_REQUEST("ParIS_wREQUEST",nPorts),
      w_GRANT("ParIS_wGRANT",nPorts),
      w_READ_OK("ParIS_wREAD_OK",nPorts),
      w_READ("ParIS_wREAD",nPorts),
      w_IDLE("ParIS_wIDLE",nPorts),
      w_DATA("ParIS_wDATA",nPorts),
      w_GND("ParIS_wGND")
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
        IMemory*  u_i_MEM = PLUGIN_MANAGER->memoryInstance(strMemIn,XID,YID,i,FIFO_IN_DEPTH);
        char strRouting[30];
        sprintf(strRouting,"%s_ROUTING",strXIN);
        IRouting* u_ROUTING = PLUGIN_MANAGER->routingInstance(strRouting,XID,YID,nPorts);
        char strIFC[20];
        sprintf(strIFC,"%s_IFC",strXIN);
        IInputFlowControl* u_IFC = PLUGIN_MANAGER->inputFlowControlInstance(strIFC,XID,YID,i);

        // Getting output units
        char strMemOut[20];
        sprintf(strMemOut,"%s_MEM_OUT",strXOUT);
        IMemory*  u_o_MEM = PLUGIN_MANAGER->memoryInstance(strMemOut,XID,YID,i,FIFO_OUT_DEPTH);
        char strPG[20];
        sprintf(strPG,"%s_PG",strXOUT);
        IPriorityGenerator* u_PG = PLUGIN_MANAGER->priorityGeneratorInstance(strPG,XID,YID,i,nPorts);
        char strArb[35];
        sprintf(strArb,"%s_DIST_ARBITER",strXOUT);
        IArbiter* u_ARBITER = new DistributedArbiter(strArb,nPorts,u_PG,XID,YID,i);
        char strOFC[20];
        sprintf(strOFC,"%s_OFC",strXOUT);
        // The last argument is used only in credit-based flow control
        IOutputFlowControl* u_OFC = PLUGIN_MANAGER->outputFlowControlInstance(strOFC,XID,YID,i,CREDIT);

        // Instantiate internal units
        u_XIN[i] = new XIN(strXIN,u_i_MEM,u_ROUTING,u_IFC,nPorts,XID,YID,i);
        u_XOUT[i] = new XOUT(strXOUT,u_o_MEM,u_ARBITER,u_OFC,nPorts,XID,YID,i);
    }

    // Binding ports
    for( i = 0; i < nPorts; i++ ) {
        XIN* xin = u_XIN[i];
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

        XOUT* xout = u_XOUT[i];
        // Bindig XOUT
        // System signals
        xout->i_CLK(i_CLK);
        xout->i_RST(i_RST);
        // Link signals
        xout->o_VALID(o_VALID_OUT[i]);
        xout->i_RETURN(i_RETURN_OUT[i]);
        xout->i_HOLD_SEND(w_GND);
        xout->o_DATA(o_DATA_OUT[i]);
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
    char waveName[15];
    sprintf(waveName,"ParIS_%u_%u_%u",XID,YID,nPorts);
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
    }

#endif
}

ParIS::~ParIS() {
#ifdef WAVEFORM_PARIS
    sc_close_vcd_trace_file(tf);
#endif
    for(unsigned short i = 0; i < numPorts; i++) {
        XIN* xin = u_XIN[i];
        if(xin) {
            delete xin;
        }
        XOUT* xout = u_XOUT[i];
        if(xout) {
            delete xout;
        }
    }
    u_XIN.clear();
    u_XOUT.clear();
}

void ParIS::p_DEBUG() {

    std::string str = sc_time_stamp().to_string();

    for( unsigned short i = 0; i < numPorts; i++ ) {
        for( unsigned short x = 0; x < numPorts; x++ ) {
            bool req = w_REQUEST[i][x].read();
            bool grant = w_GRANT[i][x].read();
            if(req) {
                printf("\n[Router] [%u][%u] - time: %s, Req[%u][%u]",XID,YID,str.c_str(),i,x);
            }
            if(grant) {
                printf("\n[Router] [%u][%u] - time: %s, Grant[%u][%u]",XID,YID,str.c_str(),i,x);
            }

        }
    }

    for( unsigned short i = 0; i < numPorts; i++ ) {
        if( XID == 0 && YID == 0 ) {
            Flit d = w_DATA[i].read();
            printf("\n[ParIS] PORT[%u] IDLE: %d- RD: %d- ROK: %d- DATA: %s",
                   i,w_IDLE[i].read(),w_READ[i].read(),w_READ_OK[i].read(),d.data.to_string(SC_HEX_US).c_str());
        }
    }

}

extern "C" {
    SS_EXP IRouter* new_Router(sc_simcontext* simcontext,
                              sc_module_name moduleName,
                              unsigned short int nPorts,
                              unsigned short int XID,
                              unsigned short int YID) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new ParIS(moduleName,nPorts,XID,YID);
    }
    SS_EXP void delete_Router(IRouter* router) {
        delete router;
    }
}
