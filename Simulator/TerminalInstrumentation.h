#ifndef __TERMINAL_INSTRUMENTATION_H__
#define __TERMINAL_INSTRUMENTATION_H__

#include <systemc.h>
#include "fg.h"

#include "../SoCINDefines.h"
#include "../NoC/NoC.h"
#include "../FlowControl/FlowControl.h"
#include "../PluginManager/PluginManager.h"
#include "../Memory/Memory.h"
#include "UnboundedFifo.h"
#include "FlowGenerator.h"

//#define USE_OLD_FG

class TerminalInstrumentation : public SoCINModule {
public:
    // INTERFACE
    // System signals
    sc_in<bool>          i_CLK;
    sc_in<bool>          i_RST;
    sc_in<unsigned long> i_CLK_CYCLES;

    // Link signals
    sc_out<Flit>    o_DATA_OUT;   // Output data channel
    sc_out<bool>    o_VALID_OUT;  // Data validation
    sc_in<bool>     i_RETURN_OUT; // Return (ack or credit)
    sc_in<Flit>     i_DATA_IN;    // Input data channel
    sc_in<bool>     i_VALID_IN;   // Data validation
    sc_out<bool>    o_RETURN_IN;  // Return (ack or credit)

    // Status signals
    sc_out<bool>            o_END_OF_TRANSMISSION;
    sc_out<unsigned int>    o_NUMBER_OF_PACKETS_SENT;
    sc_out<unsigned int>    o_NUMBER_OF_PACKETS_RECEIVED;

    sc_vector<sc_out<bool> > o_VC; // Virtual channel selector of the packet sent

    // Internal signals
    sc_signal<Flit> w_DATA_SEND;
    sc_signal<bool> w_WRITE_OK_SEND;
    sc_signal<bool> w_WRITE_SEND;
    sc_signal<bool> w_READ_OK_SEND;
    sc_signal<bool> w_READ_SEND;

    sc_signal<Flit> w_DATA_RECEIVE;
    sc_signal<bool> w_WRITE_OK_RECEIVE;
    sc_signal<bool> w_WRITE_RECEIVE;
    sc_signal<bool> w_READ_OK_RECEIVE;
    sc_signal<bool> w_READ_RECEIVE;

    // Internal data structures
    unsigned short int TG_ID;

    IInputFlowControl*  u_IFC;
    IOutputFlowControl* u_OFC;
    UnboundedFifo* u_FIFO_OUT;

    void p_debug(void) {
        if ((TG_ID)==0) {
            if (w_WRITE_SEND.read()) {
                printf("\n");
                printf("snd_wok = %d",(unsigned int) w_WRITE_OK_SEND.read());
                printf("\tsnd_wr = %d",(unsigned int) w_WRITE_SEND.read());
//                printf("\tsnd_data = 0x%8X",(unsigned int) snd_data_wire.read());
                printf("\tsnd_rok = %d",(unsigned int) w_READ_OK_SEND.read());
                printf("\tsnd_rd = %d",(unsigned int) w_READ_SEND.read());
                printf("\tout_data = 0x%8X",(unsigned int) o_DATA_OUT.read().data.to_int());
            }
        }
    }

    SC_HAS_PROCESS(TerminalInstrumentation);
    //////////////////////////////////////////////////////////////////////////////
    TerminalInstrumentation(sc_module_name nm,
        unsigned short int TG_ID, INoC::TopologyType topologyType) :
        SoCINModule(nm),
        TG_ID(TG_ID)
    //////////////////////////////////////////////////////////////////////////////
    {
        if(NUM_VC>1) {
            o_VC.init( ceil(log2(NUM_VC)) );
        }

        u_FIFO_OUT = new UnboundedFifo("FifoOutTG"); // Unbounded Fifo Packet Source
        u_FIFO_OUT->id = TG_ID;
        u_FIFO_OUT->i_CLK(i_CLK);
        u_FIFO_OUT->i_RST(i_RST);
        u_FIFO_OUT->i_DATA_IN(w_DATA_SEND);
        u_FIFO_OUT->i_WR(w_WRITE_SEND);
        u_FIFO_OUT->i_RD(w_READ_SEND);
        u_FIFO_OUT->o_WR_OK(w_WRITE_OK_SEND);
        u_FIFO_OUT->o_RD_OK(w_READ_OK_SEND);
        u_FIFO_OUT->o_DATA_OUT(o_DATA_OUT);

        //////////////////////////////////////////////
        u_OFC = PLUGIN_MANAGER->outputFlowControlInstance("TG_OFC",0,0,FIFO_IN_DEPTH);
        //////////////////////////////////////////////
        u_OFC->i_CLK(i_CLK);
        u_OFC->i_RST(i_RST);
        u_OFC->o_VALID(o_VALID_OUT);
        u_OFC->i_RETURN(i_RETURN_OUT);
        u_OFC->i_READ_OK(w_READ_OK_SEND);
        u_OFC->o_READ(w_READ_SEND);

        IMemory* u_FIFO_IN = PLUGIN_MANAGER->memoryInstance("Fifo_NULL_InTG",TG_ID,0,0); // No memory - FIFO_NULL
        u_FIFO_IN->i_CLK(i_CLK);
        u_FIFO_IN->i_RST(i_RST);
        u_FIFO_IN->i_DATA(i_DATA_IN);
        u_FIFO_IN->i_WRITE(w_WRITE_RECEIVE);
        u_FIFO_IN->i_READ(w_READ_RECEIVE);
        u_FIFO_IN->o_WRITE_OK(w_WRITE_OK_RECEIVE);
        u_FIFO_IN->o_READ_OK(w_READ_OK_RECEIVE);
        u_FIFO_IN->o_DATA(w_DATA_RECEIVE);

        //////////////////////////////////////////////
        u_IFC = PLUGIN_MANAGER->inputFlowControlInstance("TG_IFC",0,0);
        //////////////////////////////////////////////
        u_IFC->i_CLK(i_CLK);
        u_IFC->i_RST(i_RST);
        u_IFC->i_VALID(i_VALID_IN);
        u_IFC->o_RETURN(o_RETURN_IN);
        u_IFC->i_READ_OK(w_READ_OK_RECEIVE);
        u_IFC->i_READ(w_READ_RECEIVE);
        u_IFC->i_WRITE_OK(w_WRITE_OK_RECEIVE);
        u_IFC->o_WRITE(w_WRITE_RECEIVE);
        u_IFC->i_DATA(w_DATA_RECEIVE);
#ifdef USE_OLD_FG
        //////////////////////////////////
        fg *fg0 = new fg("fg0",TG_ID,topologyType,u_IFC->numberOfCyclesPerFlit());
        //////////////////////////////////
        fg0->clk(i_CLK);
        fg0->rst(i_RST);
        fg0->clock_cycles(i_CLK_CYCLES);
        fg0->snd_data(w_DATA_SEND);
        fg0->snd_wr(w_WRITE_SEND);
        fg0->snd_wok(w_WRITE_OK_SEND);
        fg0->rcv_data(w_DATA_RECEIVE);
        fg0->rcv_rok(w_READ_OK_RECEIVE);
        fg0->rcv_rd(w_READ_RECEIVE);
        fg0->eot(o_END_OF_TRANSMISSION);
        fg0->number_of_packets_sent(o_NUMBER_OF_PACKETS_SENT);
        fg0->number_of_packets_received(o_NUMBER_OF_PACKETS_RECEIVED);
        if(NUM_VC>1)
            fg0->o_VC(o_VC);
#else
        FlowGenerator* fg0 = new FlowGenerator("FlowGenerator",TG_ID,topologyType,u_IFC->numberOfCyclesPerFlit());
        fg0->i_CLK(i_CLK);
        fg0->i_RST(i_RST);
        fg0->i_CLK_CYCLES(i_CLK_CYCLES);
        fg0->o_DATA_SEND(w_DATA_SEND);
        fg0->o_WRITE_SEND(w_WRITE_SEND);
        fg0->i_WRITE_OK_SEND(w_WRITE_OK_SEND);
        fg0->i_DATA_RECEIVE(w_DATA_RECEIVE);
        fg0->i_READ_OK_RECEIVE(w_READ_OK_RECEIVE);
        fg0->o_READ_RECEIVE(w_READ_RECEIVE);
        fg0->o_END_OF_TRANSMISSION(o_END_OF_TRANSMISSION);
        fg0->o_NUMBER_OF_PACKETS_SENT(o_NUMBER_OF_PACKETS_SENT);
        fg0->o_NUMBER_OF_PACKETS_RECEIVED(o_NUMBER_OF_PACKETS_RECEIVED);
        if( NUM_VC > 1 ) {
            fg0->o_VC_SEND(o_VC);
        }
#endif
    }

    ModuleType moduleType() const { return SoCINModule::TTerminalInstrumentation; }
    const char* moduleName() const { return "TerminalInstrumentation"; }

    ~TerminalInstrumentation() {
        delete u_FIFO_OUT;
    }

};
#endif // __TERMINAL_INSTRUMENTATION_H__
