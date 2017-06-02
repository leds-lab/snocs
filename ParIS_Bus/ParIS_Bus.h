#ifndef __PARIS_BUS_H__
#define __PARIS_BUS_H__

#include "../src/Router.h"

class ParIS_none_VC_Bus : public IRouter {
public:
    // Internal signals
    // Wires
    // IFCs -> FIFOs IN
    sc_vector<sc_signal<bool> > w_WRITE_IN;     // N ports
    sc_vector<sc_signal<bool> > w_WRITE_OK_IN;  // N ports

    // Fifo INs -> ODS_BUS
    // Fifo INs -> ICs
    // IRS Bus  -> Fifo INs
    sc_vector<sc_signal<Flit> > w_DATA_OUT_FIFO_IN;    // N ports
    sc_vector<sc_signal<bool> > w_READ_OK_OUT_FIFO_IN; // N ports

    // ICs     -> ARBITER (Req0 .. Req1 .. ReqN-1)
    // ICs     -> Routing (MUX)
    sc_vector<sc_signal<bool> >             w_REQUEST;     // N ports
    sc_vector<sc_vector<sc_signal<bool> > > w_DESTINATION; // Dest[Ports][Bit]
    sc_vector<sc_signal<bool> > w_DEST_GRANTED; // [Bit]

    // ARBITER -> ODS - OWS - IRS
    sc_signal<bool>             w_IDLE;
    sc_vector<sc_signal<bool> > w_GRANT;    // N ports

    // ODS BUS -> Fifo OUTs
    sc_signal<Flit> w_DATA_OUT;

    // OWS -> Fifo OUTs (wr)
    sc_vector<sc_signal<bool> > w_WRITE_OUT; // N ports

    // IRS -> Fifo INs (rd)
    sc_vector<sc_signal<bool> > w_IRS_OUT_FIFO_IN;  // N ports

    // Fifo OUTs -> IRS
    sc_vector<sc_signal<bool> > w_WRITE_OK_OUT; // N ports

    // Fifo OUTs -> OFCs
    sc_vector<sc_signal<bool> > w_READ_OK_OUT;  // N ports
    sc_vector<sc_signal<bool> > w_READ_OUT;     // N ports

    // OWS internal signal to connect mux to demux
    sc_signal<bool> w_OWS_INTERNAL;

    // IRS internal signal to connect mux to demux
    sc_signal<bool> w_IRS_INTERNAL;

    sc_vector<sc_signal<bool> > w_ROK_FIFO_IN_AND_ARBREQUEST;


    // Internal Units - Input and output module
    IArbiter*                         u_ARBITER;

    std::vector<IInputFlowControl* >  u_IFC;
    std::vector<IMemory *>            u_MEM_IN;
    std::vector<InputControllerBus *> u_IC;
    std::vector<And *>                u_ROK_FIFO_IN_AND_IC_REQUEST;
    OneHotMux<Flit>*                  u_ODS_MUX;
    OneHotMux<bool>*                  u_OWS_MUX;
    OneHotDemux<bool>*                u_OWS_DEMUX;
    OneHotMux<bool>*                  u_IRS_MUX;
    OneHotDemux<bool>*                u_IRS_DEMUX;
    std::vector<IMemory *>            u_MEM_OUT;
    std::vector<IOutputFlowControl* > u_OFC;

    void p_SELECT_ROUTING();

    SC_HAS_PROCESS(ParIS_none_VC_Bus);
    ParIS_none_VC_Bus(sc_module_name mn,
              unsigned short nPorts);

    const char* moduleName() const { return "ParIS_none_VC_Bus"; }
    ~ParIS_none_VC_Bus();
};

#endif // __PARIS_BUS_H__
