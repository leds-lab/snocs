#include "../NoC/NoC.h"
#include "../StopSim/StopSim.h"
#include "../SystemSignals/SystemSignals.h"

#include "../PluginManager/PluginManager.h"

// TEMP
#include "tg.h"
#include "tm_single.h"

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

int sc_main(int argc, char* argv[]) {

//    std::cout << " >>> Main - SimContext: " << static_cast<void*>(sc_curr_simcontext)
//              << ", pAddr: " << &sc_curr_simcontext << std::endl;

    // Defining dimension
    X_SIZE = 2;
    Y_SIZE = 2;

    FLIT_WIDTH = 34;

    INoC* u_NETWORK = NULL;

    char confFile[30] = "simconf.conf";

    char* pluginsDir = argv[1];
    char* workDir = argv[2];

    if( PLUGIN_MANAGER->parseFile(confFile,pluginsDir) ) {
        if( PLUGIN_MANAGER->loadPlugins() ) {
            std::cout << "Plugins loaded!" << std::endl;
            u_NETWORK = PLUGIN_MANAGER->nocInstance("NoC");
            std::cout << "NoC Instantiated!" << std::endl;

        } else {
            std::cout << "Fail on load plugins ;-(" << std::endl;
        }
    } else {
        std::cout << "Conf file parse fail - oops" << std::endl;
    }

    if( u_NETWORK == NULL ) {
        delete PLUGIN_MANAGER;
        return -1;
    }

    // Obtaining the operation clock frequency
    if (argc >= 3) {
        CLK_PERIOD = atof(argv[3]);
        if(CLK_PERIOD == 0) {
            CLK_PERIOD = 1;
        }
    } else {
        CLK_PERIOD = 1;
    }
    // System signals
    sc_clock                 w_CLK("clk", CLK_PERIOD, SC_NS); // System clock | Tclk=1 ns
    sc_signal<bool>          w_RST;                           // Reset
    sc_signal<unsigned long long> w_GLOBAL_CLOCK;                  // Number of cycles

    // NoC terminal (0,0)
    sc_signal<Flit> L_0_0_in_data_wire;
    sc_signal<bool> L_0_0_in_val_wire;
    sc_signal<bool> L_0_0_in_ret_wire;
    sc_signal<Flit> L_0_0_out_data_wire;
    sc_signal<bool> L_0_0_out_val_wire;
    sc_signal<bool> L_0_0_out_ret_wire;

    // NoC terminal (0,1)
    sc_signal<Flit> L_0_1_in_data_wire;
    sc_signal<bool> L_0_1_in_val_wire;
    sc_signal<bool> L_0_1_in_ret_wire;
    sc_signal<Flit> L_0_1_out_data_wire;
    sc_signal<bool> L_0_1_out_val_wire;
    sc_signal<bool> L_0_1_out_ret_wire;

    // NoC terminal (1,0)
    sc_signal<Flit> L_1_0_in_data_wire;
    sc_signal<bool> L_1_0_in_val_wire;
    sc_signal<bool> L_1_0_in_ret_wire;
    sc_signal<Flit> L_1_0_out_data_wire;
    sc_signal<bool> L_1_0_out_val_wire;
    sc_signal<bool> L_1_0_out_ret_wire;

    // NoC terminal (1,1)
    sc_signal<Flit> L_1_1_in_data_wire;
    sc_signal<bool> L_1_1_in_val_wire;
    sc_signal<bool> L_1_1_in_ret_wire;
    sc_signal<Flit> L_1_1_out_data_wire;
    sc_signal<bool> L_1_1_out_val_wire;
    sc_signal<bool> L_1_1_out_ret_wire;

    // Status signals of the traffic generators attached to the terminals of the NoC
    sc_signal<bool>         TG_0_0_eot_wire;
    sc_signal<bool>         TG_0_1_eot_wire;
    sc_signal<bool>         TG_1_0_eot_wire;
    sc_signal<bool>         TG_1_1_eot_wire;
    sc_signal<unsigned int> TG_0_0_number_of_packets_sent_wire;
    sc_signal<unsigned int> TG_0_1_number_of_packets_sent_wire;
    sc_signal<unsigned int> TG_1_0_number_of_packets_sent_wire;
    sc_signal<unsigned int> TG_1_1_number_of_packets_sent_wire;
    sc_signal<unsigned int> TG_0_0_number_of_packets_received_wire;
    sc_signal<unsigned int> TG_0_1_number_of_packets_received_wire;
    sc_signal<unsigned int> TG_1_0_number_of_packets_received_wire;
    sc_signal<unsigned int> TG_1_1_number_of_packets_received_wire;

    // Status signal saying that stopsim is ready to stop simulation
    sc_signal<bool> w_EOS;

    //////////////////////////////////////////////////////////////////////////////
    SystemSignals *u_SYS_SIGNALS = new SystemSignals("SystemSignals");
    //////////////////////////////////////////////////////////////////////////////
    u_SYS_SIGNALS->i_CLK(w_CLK);
    u_SYS_SIGNALS->o_RST(w_RST);
    u_SYS_SIGNALS->o_GLOBAL_CLOCK(w_GLOBAL_CLOCK);

    //////////////////////////////////////////////////////////////////////////////
    StopSim* u_STOP = new StopSim("StopSim",(unsigned short)(X_SIZE*Y_SIZE),(char *)"stopsim");
    //////////////////////////////////////////////////////////////////////////////
    u_STOP->i_CLK(w_CLK);
    u_STOP->i_RST(w_RST);
    u_STOP->o_EOS(w_EOS);
    u_STOP->i_CLK_CYCLES(w_GLOBAL_CLOCK);

    u_STOP->i_TG_NUM_PACKETS_SENT[0](TG_0_0_number_of_packets_sent_wire);
    u_STOP->i_TG_NUM_PACKETS_SENT[1](TG_1_0_number_of_packets_sent_wire);
    u_STOP->i_TG_NUM_PACKETS_SENT[2](TG_0_1_number_of_packets_sent_wire);
    u_STOP->i_TG_NUM_PACKETS_SENT[3](TG_1_1_number_of_packets_sent_wire);
    u_STOP->i_TG_NUM_PACKETS_RECEIVED[0](TG_0_0_number_of_packets_received_wire);
    u_STOP->i_TG_NUM_PACKETS_RECEIVED[1](TG_1_0_number_of_packets_received_wire);
    u_STOP->i_TG_NUM_PACKETS_RECEIVED[2](TG_0_1_number_of_packets_received_wire);
    u_STOP->i_TG_NUM_PACKETS_RECEIVED[3](TG_1_1_number_of_packets_received_wire);
    u_STOP->i_TG_EOT[0](TG_0_0_eot_wire);
    u_STOP->i_TG_EOT[1](TG_1_0_eot_wire);
    u_STOP->i_TG_EOT[2](TG_0_1_eot_wire);
    u_STOP->i_TG_EOT[3](TG_1_1_eot_wire);

    //////////////////////////////////////////////////////////////////////////////
    tg *tg_0_0 = new tg("tg_0_0", 0, 0);
    //////////////////////////////////////////////////////////////////////////////
    tg_0_0->clk(w_CLK);
    tg_0_0->rst(w_RST);
    tg_0_0->clock_cycles(w_GLOBAL_CLOCK);
    tg_0_0->out_data  (L_0_0_in_data_wire );
    tg_0_0->out_val   (L_0_0_in_val_wire  );
    tg_0_0->out_ret   (L_0_0_in_ret_wire  );
    tg_0_0->in_data   (L_0_0_out_data_wire);
    tg_0_0->in_val    (L_0_0_out_val_wire );
    tg_0_0->in_ret    (L_0_0_out_ret_wire );
    tg_0_0->eot(TG_0_0_eot_wire);
    tg_0_0->number_of_packets_sent(TG_0_0_number_of_packets_sent_wire);
    tg_0_0->number_of_packets_received(TG_0_0_number_of_packets_received_wire);

    //////////////////////////////////////////////////////////////////////////////
    tg *tg_0_1 = new tg("tg_0_1", 0, 1);
    //////////////////////////////////////////////////////////////////////////////
    tg_0_1->clk(w_CLK);
    tg_0_1->rst(w_RST);
    tg_0_1->clock_cycles(w_GLOBAL_CLOCK);
    tg_0_1->out_data  (L_0_1_in_data_wire );
    tg_0_1->out_val   (L_0_1_in_val_wire  );
    tg_0_1->out_ret   (L_0_1_in_ret_wire  );
    tg_0_1->in_data   (L_0_1_out_data_wire);
    tg_0_1->in_val    (L_0_1_out_val_wire );
    tg_0_1->in_ret    (L_0_1_out_ret_wire );
    tg_0_1->eot(TG_0_1_eot_wire);
    tg_0_1->number_of_packets_sent(TG_0_1_number_of_packets_sent_wire);
    tg_0_1->number_of_packets_received(TG_0_1_number_of_packets_received_wire);

    //////////////////////////////////////////////////////////////////////////////
    tg *tg_1_0 = new tg("tg_1_0", 1, 0);
    //////////////////////////////////////////////////////////////////////////////
    tg_1_0->clk(w_CLK);
    tg_1_0->rst(w_RST);
    tg_1_0->clock_cycles(w_GLOBAL_CLOCK);
    tg_1_0->out_data  (L_1_0_in_data_wire );
    tg_1_0->out_val   (L_1_0_in_val_wire  );
    tg_1_0->out_ret   (L_1_0_in_ret_wire  );
    tg_1_0->in_data   (L_1_0_out_data_wire);
    tg_1_0->in_val    (L_1_0_out_val_wire );
    tg_1_0->in_ret    (L_1_0_out_ret_wire );
    tg_1_0->eot(TG_1_0_eot_wire);
    tg_1_0->number_of_packets_sent(TG_1_0_number_of_packets_sent_wire);
    tg_1_0->number_of_packets_received(TG_1_0_number_of_packets_received_wire);

    //////////////////////////////////////////////////////////////////////////////
    tg *tg_1_1 = new tg("tg_1_1", 1, 1);
    //////////////////////////////////////////////////////////////////////////////
    tg_1_1->clk(w_CLK);
    tg_1_1->rst(w_RST);
    tg_1_1->clock_cycles(w_GLOBAL_CLOCK);
    tg_1_1->out_data  (L_1_1_in_data_wire );
    tg_1_1->out_val   (L_1_1_in_val_wire  );
    tg_1_1->out_ret   (L_1_1_in_ret_wire  );
    tg_1_1->in_data   (L_1_1_out_data_wire);
    tg_1_1->in_val    (L_1_1_out_val_wire );
    tg_1_1->in_ret    (L_1_1_out_ret_wire );
    tg_1_1->eot(TG_1_1_eot_wire);
    tg_1_1->number_of_packets_sent(TG_1_1_number_of_packets_sent_wire);
    tg_1_1->number_of_packets_received(TG_1_1_number_of_packets_received_wire);

    //////////////////////////////////////////////////////////////////////////////
    tm_single *tm_0_0_out = new tm_single("tm_0_0_out", 0, 0,(char *) workDir, (char*)"ext_0_0_out");
    //////////////////////////////////////////////////////////////////////////////
    tm_0_0_out->clk(w_CLK);
    tm_0_0_out->rst(w_RST);
    tm_0_0_out->eos(w_EOS);
    tm_0_0_out->clock_cycles(w_GLOBAL_CLOCK);
    tm_0_0_out->data(L_0_0_out_data_wire );
    tm_0_0_out->val (L_0_0_out_val_wire  );
    tm_0_0_out->ret (L_0_0_out_ret_wire  );

    //////////////////////////////////////////////////////////////////////////////
    tm_single *tm_0_1_out = new tm_single("tm_0_1_out", 0, 1,(char *) workDir, (char*)"ext_0_1_out");
    //////////////////////////////////////////////////////////////////////////////
    tm_0_1_out->clk(w_CLK);
    tm_0_1_out->rst(w_RST);
    tm_0_1_out->eos(w_EOS);
    tm_0_1_out->clock_cycles(w_GLOBAL_CLOCK);
    tm_0_1_out->data(L_0_1_out_data_wire );
    tm_0_1_out->val (L_0_1_out_val_wire  );
    tm_0_1_out->ret (L_0_1_out_ret_wire  );

    //////////////////////////////////////////////////////////////////////////////
    tm_single *tm_1_0_out = new tm_single("tm_1_0_out", 1, 0,(char *) workDir, (char*)"ext_1_0_out");
    //////////////////////////////////////////////////////////////////////////////
    tm_1_0_out->clk(w_CLK);
    tm_1_0_out->rst(w_RST);
    tm_1_0_out->eos(w_EOS);
    tm_1_0_out->clock_cycles(w_GLOBAL_CLOCK);
    tm_1_0_out->data(L_1_0_out_data_wire );
    tm_1_0_out->val (L_1_0_out_val_wire  );
    tm_1_0_out->ret (L_1_0_out_ret_wire  );

    //////////////////////////////////////////////////////////////////////////////
    tm_single *tm_1_1_out = new tm_single("tm_1_1_out", 1, 1,(char *) workDir, (char*)"ext_1_1_out");
    //////////////////////////////////////////////////////////////////////////////
    tm_1_1_out->clk(w_CLK);
    tm_1_1_out->rst(w_RST);
    tm_1_1_out->eos(w_EOS);
    tm_1_1_out->clock_cycles(w_GLOBAL_CLOCK);
    tm_1_1_out->data(L_1_1_out_data_wire );
    tm_1_1_out->val (L_1_1_out_val_wire  );
    tm_1_1_out->ret (L_1_1_out_ret_wire  );

    //////////////////////////////////////////////////////////////////////////////
    INoC *u_NOC = u_NETWORK;
    //////////////////////////////////////////////////////////////////////////////
    // System signals
    u_NOC->i_CLK(w_CLK);
    u_NOC->i_RST(w_RST);

    // NoC terminal 0 0
    u_NOC->i_DATA_IN   [0](L_0_0_in_data_wire );
    u_NOC->i_VALID_IN  [0](L_0_0_in_val_wire  );
    u_NOC->o_RETURN_IN [0](L_0_0_in_ret_wire  );
    u_NOC->o_DATA_OUT  [0](L_0_0_out_data_wire);
    u_NOC->o_VALID_OUT [0](L_0_0_out_val_wire );
    u_NOC->i_RETURN_OUT[0](L_0_0_out_ret_wire );

    // NoC terminal 1 0
    u_NOC->i_DATA_IN   [1](L_1_0_in_data_wire );
    u_NOC->i_VALID_IN  [1](L_1_0_in_val_wire  );
    u_NOC->o_RETURN_IN [1](L_1_0_in_ret_wire  );
    u_NOC->o_DATA_OUT  [1](L_1_0_out_data_wire);
    u_NOC->o_VALID_OUT [1](L_1_0_out_val_wire );
    u_NOC->i_RETURN_OUT[1](L_1_0_out_ret_wire );

    // NoC terminal 0 1
    u_NOC->i_DATA_IN   [2](L_0_1_in_data_wire );
    u_NOC->i_VALID_IN  [2](L_0_1_in_val_wire  );
    u_NOC->o_RETURN_IN [2](L_0_1_in_ret_wire  );
    u_NOC->o_DATA_OUT  [2](L_0_1_out_data_wire);
    u_NOC->o_VALID_OUT [2](L_0_1_out_val_wire );
    u_NOC->i_RETURN_OUT[2](L_0_1_out_ret_wire );

    // NoC terminal 1 1
    u_NOC->i_DATA_IN   [3](L_1_1_in_data_wire );
    u_NOC->i_VALID_IN  [3](L_1_1_in_val_wire  );
    u_NOC->o_RETURN_IN [3](L_1_1_in_ret_wire  );
    u_NOC->o_DATA_OUT  [3](L_1_1_out_data_wire);
    u_NOC->o_VALID_OUT [3](L_1_1_out_val_wire );
    u_NOC->i_RETURN_OUT[3](L_1_1_out_ret_wire );

    // Signal tracing
    sc_trace_file *tf=sc_create_vcd_trace_file("system");
    sc_trace(tf, w_CLK           , "clk");
    sc_trace(tf, w_EOS           , "eos");
    sc_trace(tf, w_RST           , "rst");
    sc_trace(tf, w_GLOBAL_CLOCK  , "global_clock");

    sc_trace(tf, L_0_0_in_data_wire , "L_0_0_in_data");
    sc_trace(tf, L_0_0_in_val_wire  , "L_0_0_in_val");
    sc_trace(tf, L_0_0_in_ret_wire  , "L_0_0_in_ret");
    sc_trace(tf, L_0_0_out_data_wire, "L_0_0_out_data");
    sc_trace(tf, L_0_0_out_val_wire , "L_0_0_out_val");
    sc_trace(tf, L_0_0_out_ret_wire , "L_0_0_out_ret");

    sc_trace(tf, L_0_1_in_data_wire , "L_0_1_in_data");
    sc_trace(tf, L_0_1_in_val_wire  , "L_0_1_in_val");
    sc_trace(tf, L_0_1_in_ret_wire  , "L_0_1_in_ret");
    sc_trace(tf, L_0_1_out_data_wire, "L_0_1_out_data");
    sc_trace(tf, L_0_1_out_val_wire , "L_0_1_out_val");
    sc_trace(tf, L_0_1_out_ret_wire , "L_0_1_out_ret");

    sc_trace(tf, L_1_0_in_data_wire , "L_1_0_in_data");
    sc_trace(tf, L_1_0_in_val_wire  , "L_1_0_in_val");
    sc_trace(tf, L_1_0_in_ret_wire  , "L_1_0_in_ret");
    sc_trace(tf, L_1_0_out_data_wire, "L_1_0_out_data");
    sc_trace(tf, L_1_0_out_val_wire , "L_1_0_out_val");
    sc_trace(tf, L_1_0_out_ret_wire , "L_1_0_out_ret");

    sc_trace(tf, L_1_1_in_data_wire , "L_1_1_in_data");
    sc_trace(tf, L_1_1_in_val_wire  , "L_1_1_in_val");
    sc_trace(tf, L_1_1_in_ret_wire  , "L_1_1_in_ret");
    sc_trace(tf, L_1_1_out_data_wire, "L_1_1_out_data");
    sc_trace(tf, L_1_1_out_val_wire , "L_1_1_out_val");
    sc_trace(tf, L_1_1_out_ret_wire , "L_1_1_out_ret");

    sc_trace(tf, TG_0_0_eot_wire    , "TG_0_0_eot");
    sc_trace(tf, TG_0_1_eot_wire    , "TG_0_1_eot");
    sc_trace(tf, TG_1_0_eot_wire    , "TG_1_0_eot");
    sc_trace(tf, TG_1_1_eot_wire    , "TG_1_1_eot");

    std::cout << "\n\n\n//////////////////////////////////////////////" << std::endl;
    std::cout << "////////////// SoCIN Simulator  //////////////" << std::endl;
    std::cout << "////////////// Start Simulation //////////////" << std::endl;
    std::cout << "//////////////////////////////////////////////" << std::endl << std::endl << std::endl;

    // Start the simulation (the testbench will stop it)
    sc_start();

    sc_close_vcd_trace_file(tf);

    delete PLUGIN_MANAGER;

    return 0;
}
