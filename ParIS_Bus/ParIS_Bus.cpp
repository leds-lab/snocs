#include "ParIS_Bus.h"

ParIS_none_VC_Bus::ParIS_none_VC_Bus(sc_module_name mn,unsigned short nPorts)
    : IRouter(mn,nPorts,0),
      w_WRITE_IN("ParIS_none_VC_Bus_wWRITE_IN",nPorts),
      w_WRITE_OK_IN("ParIS_none_VC_Bus_wWRITE_OK_IN",nPorts),
      w_DATA_OUT_FIFO_IN("ParIS_none_VC_Bus_wDATA_OUT_FIFO_IN",nPorts),
      w_READ_OK_OUT_FIFO_IN("ParIS_none_VC_Bus_wREAD_OK_OUT_FIFO_IN",nPorts),
      w_REQUEST("ParIS_none_VC_Bus_wREQUEST",nPorts),
      w_DESTINATION("ParIS_none_VC_Bus_wDESTINATION",nPorts),
      w_DEST_GRANTED("ParIS_none_VC_Bus_wDEST_GRANTED",nPorts),
      w_IDLE("ParIS_none_VC_Bus_wIDLE"),
      w_GRANT("ParIS_none_VC_Bus_wGRANT",nPorts),
      w_DATA_OUT("ParIS_none_VC_Bus_wDATA_OUT"),
      w_WRITE_OUT("ParIS_none_VC_Bus_wWRITE_OUT",nPorts),
      w_IRS_OUT_FIFO_IN("ParIS_none_VC_Bus_wIRS_OUT_FIFO_IN",nPorts),
      w_WRITE_OK_OUT("ParIS_none_VC_Bus_wWRITE_OK_OUT",nPorts),
      w_READ_OK_OUT("ParIS_none_VC_Bus_wREAD_OK_OUT",nPorts),
      w_READ_OUT("ParIS_none_VC_Bus_wREAD_OUT",nPorts),
      w_OWS_INTERNAL("ParIS_none_VC_Bus_wOWS_INTERNAL"),
      w_IRS_INTERNAL("ParIS_none_VC_Bus_wIRS_INTERNAL"),
      w_ROK_FIFO_IN_AND_ARBREQUEST("ParIS_none_VC_Bus_wROK_FIFO_IN_AND_ARBREQUEST",nPorts),
      u_IFC(nPorts,NULL),
      u_MEM_IN(nPorts,NULL),
      u_IC(nPorts,NULL),
      u_ROK_FIFO_IN_AND_IC_REQUEST(nPorts,NULL),
      u_MEM_OUT(nPorts,NULL),
      u_OFC(nPorts,NULL)
{

    unsigned short i;
    for( i = 0; i < nPorts; i++ ) {
        w_DESTINATION[i].init(nPorts);
    }

    // Instantiating internal modules
    IPriorityGenerator* pg = PLUGIN_MANAGER->priorityGeneratorInstance("PG_BUS",ROUTER_ID,0,nPorts);
    u_ARBITER     = new DistributedArbiter("Arbiter_BUS",nPorts,pg,ROUTER_ID,0);
    u_ODS_MUX     = new OneHotMux<Flit>("ODS_BUS",nPorts);
    u_OWS_MUX     = new OneHotMux<bool>("OWS_INPUT",nPorts);
    u_OWS_DEMUX   = new OneHotDemux<bool>("OWS_OUTPUT",nPorts);
    u_IRS_MUX     = new OneHotMux<bool>("IRS_INPUT",nPorts);
    u_IRS_DEMUX   = new OneHotDemux<bool>("IRS_OUTPUT",nPorts);
    for( i = 0; i < nPorts; i++) {
        char strIfc[15];
        sprintf(strIfc,"BUS_IFC(%u)",i);
        u_IFC[i] = PLUGIN_MANAGER->inputFlowControlInstance(strIfc,ROUTER_ID,i);
        char strMemIn[16];
        sprintf(strMemIn,"BUS_MEM_IN(%u)",i);
        u_MEM_IN[i] = PLUGIN_MANAGER->memoryInstance(strMemIn,ROUTER_ID,i,FIFO_IN_DEPTH);
        char strIc[15];
        sprintf(strIc,"BUS_IC(%u)",i);
        u_IC[i] = new InputControllerBus(strIc,nPorts,ROUTER_ID,i);
        char strMemOut[17];
        sprintf(strMemOut,"BUS_MEM_OUT(%u)",i);
        u_MEM_OUT[i] = PLUGIN_MANAGER->memoryInstance(strMemOut,ROUTER_ID,i,FIFO_OUT_DEPTH);
        char strOfc[15];
        sprintf(strOfc,"BUS_OFC(%u)",i);
        u_OFC[i] = PLUGIN_MANAGER->outputFlowControlInstance(strOfc,ROUTER_ID,i,FIFO_IN_DEPTH);

        char strAndROK_ICREQ[35];
        sprintf(strAndROK_ICREQ,"ROK_FIFO_IN_AND_IC_REQUEST(%u)",i);
        u_ROK_FIFO_IN_AND_IC_REQUEST[i] = new And(strAndROK_ICREQ,2);

    }

    //-------------------- Binding ports --------------------//
//  [1] //////////////////// Arbiter ////////////////////
    u_ARBITER->i_CLK(i_CLK);
    u_ARBITER->i_RST(i_RST);
    u_ARBITER->o_GRANT(w_GRANT);
    u_ARBITER->o_IDLE(w_IDLE);
    u_ARBITER->i_REQUEST(w_REQUEST);

    // Requests binding follow inside the loop
//  [1] //////////////////// Arbiter ////////////////////

//  [2] //////////////////// SWITCHES ////////////////////
    u_ODS_MUX->i_SEL(w_GRANT);
    u_ODS_MUX->i_DATA(w_DATA_OUT_FIFO_IN);
    u_ODS_MUX->o_DATA(w_DATA_OUT);

    u_OWS_MUX->i_SEL(w_GRANT);
    u_OWS_MUX->i_DATA(w_ROK_FIFO_IN_AND_ARBREQUEST);
    u_OWS_MUX->o_DATA(w_OWS_INTERNAL);

    u_OWS_DEMUX->i_SEL(w_DEST_GRANTED);
    u_OWS_DEMUX->i_DATA(w_OWS_INTERNAL);
    u_OWS_DEMUX->o_DATA(w_WRITE_OUT);

    u_IRS_MUX->i_SEL(w_DEST_GRANTED);
    u_IRS_MUX->i_DATA(w_WRITE_OK_OUT);
    u_IRS_MUX->o_DATA(w_IRS_INTERNAL);

    u_IRS_DEMUX->i_SEL(w_GRANT);
    u_IRS_DEMUX->i_DATA(w_IRS_INTERNAL);
    u_IRS_DEMUX->o_DATA(w_IRS_OUT_FIFO_IN);
//  [2] //////////////////// SWITCHES ////////////////////

    for( i = 0; i < nPorts; i++ ) {
//  [3] //////////////////// Inputs ////////////////////
        IInputFlowControl* ifc = u_IFC[i];
        ifc->i_CLK(i_CLK);
        ifc->i_RST(i_RST);
        ifc->i_VALID(i_VALID_IN[i]);
        ifc->o_RETURN(o_RETURN_IN[i]);
        ifc->o_WRITE(w_WRITE_IN[i]);
        ifc->i_WRITE_OK(w_WRITE_OK_IN[i]);
        ifc->i_READ(w_IRS_OUT_FIFO_IN[i]);
        ifc->i_READ_OK(w_READ_OK_OUT_FIFO_IN[i]);
        ifc->i_DATA(i_DATA_IN[i]);

        IMemory* memIn = u_MEM_IN[i];
        memIn->i_CLK(i_CLK);
        memIn->i_RST(i_RST);
        memIn->o_READ_OK(w_READ_OK_OUT_FIFO_IN[i]);
        memIn->o_WRITE_OK(w_WRITE_OK_IN[i]);
        memIn->i_READ(w_IRS_OUT_FIFO_IN[i]);
        memIn->i_WRITE(w_WRITE_IN[i]);
        memIn->i_DATA(i_DATA_IN[i]);
        memIn->o_DATA(w_DATA_OUT_FIFO_IN[i]);

        InputControllerBus* ic = u_IC[i];
        ic->i_CLK(i_CLK);
        ic->i_RST(i_RST);
        ic->i_READ(w_IRS_OUT_FIFO_IN[i]);
        ic->i_READ_OK(w_READ_OK_OUT_FIFO_IN[i]);
        ic->i_DATA(w_DATA_OUT_FIFO_IN[i]);
        ic->i_IDLE(w_IDLE);
        ic->o_REQUEST(w_REQUEST[i]);
        ic->o_DESTINATION(w_DESTINATION[i]);

        And* memInAndIcReq = u_ROK_FIFO_IN_AND_IC_REQUEST[i];
        memInAndIcReq->i_DATA[0](w_READ_OK_OUT_FIFO_IN[i]);
        memInAndIcReq->i_DATA[1](w_REQUEST[i]);
        memInAndIcReq->o_DATA(w_ROK_FIFO_IN_AND_ARBREQUEST[i]);
//  [3] //////////////////// Inputs ////////////////////

//  [4] //////////////////// Outputs ////////////////////
        IMemory* memOut = u_MEM_OUT[i];
        memOut->i_CLK(i_CLK);
        memOut->i_RST(i_RST);
        memOut->o_READ_OK(w_READ_OK_OUT[i]);
        memOut->o_WRITE_OK(w_WRITE_OK_OUT[i]);
        memOut->i_READ(w_READ_OUT[i]);
        memOut->i_WRITE(w_WRITE_OUT[i]);
        memOut->i_DATA(w_DATA_OUT);
        memOut->o_DATA(o_DATA_OUT[i]);

        IOutputFlowControl* ofc = u_OFC[i];
        ofc->i_CLK(i_CLK);
        ofc->i_RST(i_RST);
        ofc->o_VALID(o_VALID_OUT[i]);
        ofc->i_RETURN(i_RETURN_OUT[i]);
        ofc->o_READ(w_READ_OUT[i]);
        ofc->i_READ_OK(w_READ_OK_OUT[i]);
//  [4] //////////////////// Outputs ////////////////////
    }

    SC_METHOD(p_SELECT_ROUTING);
    for( i = 0; i < nPorts; i++ ) {
        sensitive << w_GRANT[i];
        for( unsigned short x = 0; x < nPorts; x++ ) {
            sensitive << w_DESTINATION[i][x];
        }
    }
}

void ParIS_none_VC_Bus::p_SELECT_ROUTING() {

    unsigned short i;
    for( i = 0; i < numPorts; i++) {
        if( w_GRANT[i].read() == 1 ) {
            for(unsigned short x = 0; x < numPorts; x++) {
                w_DEST_GRANTED[x].write( w_DESTINATION[i][x].read() );
            }
        }
    }
}

ParIS_none_VC_Bus::~ParIS_none_VC_Bus() {

    delete u_ARBITER;
    for( unsigned short i  = 0; i < numPorts; i++ ) {
        delete u_IC[i];
        delete u_ROK_FIFO_IN_AND_IC_REQUEST[i];
    }

    delete u_ODS_MUX;
    delete u_OWS_MUX;
    delete u_OWS_DEMUX;
    delete u_IRS_MUX;
    delete u_IRS_DEMUX;
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
                               unsigned short int ) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        if( nVirtualChannels > 1 ) {
            std::cout << "\n\n\t\tBus with VC Not implemented!" << std::endl << std::endl;
            return NULL;
        } else {
            return new ParIS_none_VC_Bus(moduleName,nPorts);
        }

    }
    SS_EXP void delete_Router(IRouter* router) {
        delete router;
    }
}
