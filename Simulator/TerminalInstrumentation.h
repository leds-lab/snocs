#ifndef __TERMINAL_INSTRUMENTATION_H__
#define __TERMINAL_INSTRUMENTATION_H__

#include <systemc.h>
#include "fg.h"

#include "../SoCINDefines.h"
#include "../FlowControl/FlowControl.h"
#include "../PluginManager/PluginManager.h"
#include "UnboundedFifo.h"

class TerminalInstrumentation : public sc_module {
public:
    // INTERFACE
    // System signals
    sc_in<bool>          clk;
    sc_in<bool>          rst;
    sc_in<unsigned long> clock_cycles;

    // Link signals
    sc_out<Flit>    out_data;   // Output data channel
    sc_out<bool>    out_val;    // Data validation
    sc_in<bool>     out_ret;    // Return (ack or credit)
    sc_in<Flit>     in_data;    // Input data channel
    sc_in<bool>     in_val;     // Data validation
    sc_out<bool>    in_ret;     // Return (ack or credit)

    // Status signals
    sc_out<bool>            eot;
    sc_out<unsigned int>    number_of_packets_sent;
    sc_out<unsigned int>    number_of_packets_received;

    sc_vector<sc_out<bool> > o_VC;

    // Internal signals
    sc_signal<Flit> snd_data_wire;
    sc_signal<bool> snd_wok_wire;
    sc_signal<bool> snd_wr_wire;
    sc_signal<bool> snd_rok_wire;
    sc_signal<bool> snd_rd_wire;

    sc_signal<Flit> rcv_data_wire;
    sc_signal<bool> rcv_wok_wire;
    sc_signal<bool> rcv_wr_wire;
    sc_signal<bool> rcv_rok_wire;
    sc_signal<bool> rcv_rd_wire;

    // Internal data structures
    unsigned short int TG_ID;

    IInputFlowControl*  u_IFC;
    IOutputFlowControl* u_OFC;
    UnboundedFifo* u_FIFO_IN;
    UnboundedFifo* u_FIFO_OUT;

    void p_debug(void) {
        if ((TG_ID)==0) {
            if (snd_wr_wire.read()) {
                printf("\n");
                printf("snd_wok = %d",(unsigned int) snd_wok_wire.read());
                printf("\tsnd_wr = %d",(unsigned int) snd_wr_wire.read());
//                printf("\tsnd_data = 0x%8X",(unsigned int) snd_data_wire.read());
                printf("\tsnd_rok = %d",(unsigned int) snd_rok_wire.read());
                printf("\tsnd_rd = %d",(unsigned int) snd_rd_wire.read());
                printf("\tout_data = 0x%8X",(unsigned int) out_data.read().data.to_int());
            }
        }
    }

    SC_HAS_PROCESS(TerminalInstrumentation);
    //////////////////////////////////////////////////////////////////////////////
    TerminalInstrumentation(sc_module_name nm,
        unsigned short int TG_ID, bool packetFormat3D) :
        sc_module(nm),
        TG_ID(TG_ID)
    //////////////////////////////////////////////////////////////////////////////
    {
        if(NUM_VC>1) {
            o_VC.init( ceil(log2(NUM_VC)) );
        }

        u_FIFO_OUT = new UnboundedFifo("FifoOutTG");
        u_FIFO_OUT->i_CLK(clk);
        u_FIFO_OUT->i_RST(rst);
        u_FIFO_OUT->i_DATA_IN(snd_data_wire);
        u_FIFO_OUT->i_WR(snd_wr_wire);
        u_FIFO_OUT->i_RD(snd_rd_wire);
        u_FIFO_OUT->o_WR_OK(snd_wok_wire);
        u_FIFO_OUT->o_RD_OK(snd_rok_wire);
        u_FIFO_OUT->o_DATA_OUT(out_data);

        //////////////////////////////////////////////
        u_OFC = PLUGIN_MANAGER->outputFlowControlInstance("TG_OFC",0,0,FIFO_IN_DEPTH);
        //////////////////////////////////////////////
        u_OFC->i_CLK(clk);
        u_OFC->i_RST(rst);
        u_OFC->o_VALID(out_val);
        u_OFC->i_RETURN(out_ret);
        u_OFC->i_READ_OK(snd_rok_wire);
        u_OFC->o_READ(snd_rd_wire);

        u_FIFO_IN = new UnboundedFifo("FifoInTG");
        u_FIFO_IN->i_CLK(clk);
        u_FIFO_IN->i_RST(rst);
        u_FIFO_IN->i_DATA_IN(in_data);
        u_FIFO_IN->i_WR(rcv_wr_wire);
        u_FIFO_IN->i_RD(rcv_rd_wire);
        u_FIFO_IN->o_WR_OK(rcv_wok_wire);
        u_FIFO_IN->o_RD_OK(rcv_rok_wire);
        u_FIFO_IN->o_DATA_OUT(rcv_data_wire);

        //////////////////////////////////////////////
        u_IFC = PLUGIN_MANAGER->inputFlowControlInstance("TG_IFC",0,0);
        //////////////////////////////////////////////
        u_IFC->i_CLK(clk);
        u_IFC->i_RST(rst);
        u_IFC->i_VALID(in_val);
        u_IFC->o_RETURN(in_ret);
        u_IFC->i_READ_OK(rcv_rok_wire);
        u_IFC->i_READ(rcv_rd_wire);
        u_IFC->i_WRITE_OK(rcv_wok_wire);
        u_IFC->o_WRITE(rcv_wr_wire);
        u_IFC->i_DATA(rcv_data_wire);

        //////////////////////////////////
        fg *fg0 = new fg("fg0",TG_ID,packetFormat3D,u_IFC->numberOfCyclesPerFlit());
        //////////////////////////////////
        fg0->clk(clk);
        fg0->rst(rst);
        fg0->clock_cycles(clock_cycles);
        fg0->snd_data(snd_data_wire);
        fg0->snd_wr(snd_wr_wire);
        fg0->snd_wok(snd_wok_wire);
        fg0->rcv_data(rcv_data_wire);
        fg0->rcv_rok(rcv_rok_wire);
        fg0->rcv_rd(rcv_rd_wire);
        fg0->eot(eot);
        fg0->number_of_packets_sent(number_of_packets_sent);
        fg0->number_of_packets_received(number_of_packets_received);
        if(NUM_VC>1)
            fg0->o_VC(o_VC);

    }

};
#endif // __TERMINAL_INSTRUMENTATION_H__
