#include "../NoC/NoC.h"
#include "../StopSim/StopSim.h"
#include "../SystemSignals/SystemSignals.h"
#include "../TrafficMeter/TrafficMeter.h"

#include "../PluginManager/PluginManager.h"

// TEMP
#include "tg.h"

#include <systemc>

#include <ctime>

using namespace sc_core;
using namespace sc_dt;

// Forward declaration of functions below the main
unsigned int setupSimulator(int argc, char* argv[]);
void generateListNodesGtkwave();
char *print_time(unsigned long long total_sec);


// Messages to setup of the simulator
const char* SETUP_MESSAGES[] =
{"Setup successfully performed!", // [0]
 "Fail on load plugins ;-(",      // [1]
 "Conf file parse fail - oops"    // [2]
};

int sc_main(int argc, char* argv[]) {

    if(argc < 2) {
        std::cout << "Simulator <tClk_in_ns> <work_dir> <plugins_dir>\n" \
                     "Using default values:\n" \
                     "tClk: 1 ns\n" \
                     "work_dir: ./work\n" \
                     "plugins_dir: ./plugins" << std::endl;
    } else {
        std::cout << "Simulator <tClk> <work_dir> <plugins_dir>";
        switch(argc) {
            case 2:
                std::cout << "\ntClk: " << argv[1] << " ns\n" \
                          << "work_dir: ./work (default)\n" \
                             "plugins_dir: ./plugins (default)" << std::endl;
                break;
            case 3:
                std::cout << "\ntClk: " << argv[1] << " ns\n" \
                          << "work_dir: " << argv[2] << "\n" \
                             "plugins_dir: ./plugins (default)" << std::endl;
                break;
            case 4:
                std::cout << "\ntClk: " << argv[1] << " ns\n" \
                          << "work_dir: " << argv[2] << "\n" \
                             "plugins_dir: "<< argv[3] << std::endl;
                break;
            default:
                std::cout << "\ntClk: " << argv[1] << " ns\n" \
                          << "work_dir: " << argv[2] << "\n" \
                             "plugins_dir: "<< argv[3] << "\n" \
                             "Additional parameters unused." << std::endl;
                break;
        }
    }

    unsigned int setupCode = setupSimulator(argc,argv);

    if( setupCode >= sizeof(SETUP_MESSAGES) / sizeof(SETUP_MESSAGES[0]) ) {
        std::cout << "Error on setup - message undefined." << std::endl;
    } else {
        std::cout << SETUP_MESSAGES[setupCode] << std::endl;
    }

    if( setupCode != 0) {
        return -1;
    }

    INoC* u_NOC = PLUGIN_MANAGER->nocInstance("NoC");

    if( u_NOC == NULL ) {
        delete PLUGIN_MANAGER;
        return -1;
    }

    // ------------------- Establishing system -------------------

    printf("\nTclk = %.0f ns", CLK_PERIOD);

    // System signals
    sc_clock                 w_CLK("CLK", CLK_PERIOD, SC_NS); // System clock | Tclk=1 ns
    sc_signal<bool>          w_RST;                           // Reset
    sc_signal<unsigned long> w_GLOBAL_CLOCK;                  // Number of cycles

    unsigned short numElements = X_SIZE*Y_SIZE;

    // Wires to connect System Components to the network - Transmission interface
    sc_vector<sc_signal<Flit> > w_IN_DATA("w_IN_DATA",numElements);        // Network data input
    sc_vector<sc_signal<bool> > w_IN_VALID("w_IN_VALID",numElements);      // Network input flow control
    sc_vector<sc_signal<bool> > w_IN_RETURN("w_IN_RETURN",numElements);    // Network input flow control
    sc_vector<sc_signal<Flit> > w_OUT_DATA("w_OUT_DATA",numElements);      // Network data output
    sc_vector<sc_signal<bool> > w_OUT_VALID("w_OUT_VALID",numElements);    // Network output flow control
    sc_vector<sc_signal<bool> > w_OUT_RETURN("w_OUT_RETURN",numElements);  // Network output flow control

    // Used only with virtual channels
    sc_vector<sc_vector<sc_signal<bool> > > w_IN_VC_SEL("w_IN_VC_SEL");
    sc_vector<sc_vector<sc_signal<bool> > > w_OUT_VC_SEL("w_OUT_VC_SEL");

    // Status signals of the traffic generators attached to the terminals of the NoC
    sc_vector<sc_signal<bool> >         w_TG_EOT("w_TG_EOT",numElements);
    sc_vector<sc_signal<unsigned int> > w_TG_NUM_PACKETS_SENT("w_TG_NUM_PACKETS_SENT",numElements);
    sc_vector<sc_signal<unsigned int> > w_TG_NUM_PACKETS_RECEIVED("w_TG_NUM_PACKETS_RECEIVED",numElements);

    // Configuring wires to virtual channels
    unsigned short vcWidth = 0;
    if( NUM_VC > 1) {
        vcWidth = ceil(log2(NUM_VC));
        w_IN_VC_SEL.init(numElements);
        w_OUT_VC_SEL.init(numElements);
        for( unsigned int r = 0; r < numElements; r++ ) {
            w_IN_VC_SEL[r].init(vcWidth);
            w_OUT_VC_SEL[r].init(vcWidth);
        }
    }

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

    //////////////////////////////////////////////////////////////////////////////
    INoC_VC *u_NOC_VC = dynamic_cast<INoC_VC *>(u_NOC);
    //////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    std::vector<TrafficMeter *> u_TMs(numElements,NULL);
    //////////////////////////////////////////////////////////////////////////////

    // System signals
    u_NOC->i_CLK(w_CLK);
    u_NOC->i_RST(w_RST);

    // Instantiating System Components (TGs, TMs) & binding dynamic ports
    for( unsigned short x = 0; x < X_SIZE; x++ ) {
        for( unsigned short y = 0; y < Y_SIZE; y++ ) {
            // Get the router ID according X and Y network position to configure the system
            unsigned short elementId = COORDINATE_TO_ID(x,y);

            // Assembling TG name
            char strTgName[10];
            sprintf(strTgName,"TG_%u_%u",x,y);
            // Instantiate TG
            tg* u_TG = new tg(strTgName,x,y);

            // Assembling TM name
            char strTmName[10];
            sprintf(strTmName,"TM_%u_%u",x,y);
            // Assembling out filename
            char* outFilename = new char[20];
            sprintf(outFilename,"ext_%u_%u_out",x,y);
            // Instantiate TM
            TrafficMeter* u_TM = new TrafficMeter(strTmName,WORK_DIR,outFilename);
            u_TMs[elementId] = u_TM;

            //------------- Binding signals -------------//

            //------------- Binding TG -------------//
            // System signals
            u_TG->clk(w_CLK);
            u_TG->rst(w_RST);
            u_TG->clock_cycles(w_GLOBAL_CLOCK);
            // Connections with routers
            // The data that outgoing of the network, incoming in the traffic generator and vice-versa
            u_TG->in_data(w_OUT_DATA[elementId]);
            u_TG->in_val(w_OUT_VALID[elementId]);
            u_TG->in_ret(w_OUT_RETURN[elementId]);
            u_TG->out_data(w_IN_DATA[elementId]);
            u_TG->out_val(w_IN_VALID[elementId]);
            u_TG->out_ret(w_IN_RETURN[elementId]);
            // Status signals to simulation control
            u_TG->eot(w_TG_EOT[elementId]);
            u_TG->number_of_packets_sent(w_TG_NUM_PACKETS_SENT[elementId]);
            u_TG->number_of_packets_received(w_TG_NUM_PACKETS_RECEIVED[elementId]);
            if(NUM_VC>1)
                u_TG->o_VC(w_IN_VC_SEL[elementId]);

            //------------- Binding TM -------------//
            u_TM->i_CLK(w_CLK);
            u_TM->i_RST(w_RST);
            u_TM->i_EOS(w_EOS);
            u_TM->i_CLK_CYCLES(w_GLOBAL_CLOCK);
            u_TM->i_DATA(w_OUT_DATA[elementId]);
            u_TM->i_VALID(w_OUT_VALID[elementId]);
            u_TM->i_RETURN(w_OUT_RETURN[elementId]);
            if(NUM_VC>1)
                u_TM->i_VC_SEL(w_OUT_VC_SEL[elementId]);

            //------------- Binding NoC -------------//
            u_NOC->i_DATA_IN   [elementId](w_IN_DATA[elementId]);
            u_NOC->i_VALID_IN  [elementId](w_IN_VALID[elementId]);
            u_NOC->o_RETURN_IN [elementId](w_IN_RETURN[elementId]);
            u_NOC->o_DATA_OUT  [elementId](w_OUT_DATA[elementId]);
            u_NOC->o_VALID_OUT [elementId](w_OUT_VALID[elementId]);
            u_NOC->i_RETURN_OUT[elementId](w_OUT_RETURN[elementId]);
            if( NUM_VC > 1 ) {
                if( u_NOC_VC != NULL ) {
                    u_NOC_VC->i_VC_SELECTOR[elementId](w_IN_VC_SEL[elementId]);
                    u_NOC_VC->o_VC_SELECTOR[elementId](w_OUT_VC_SEL[elementId]);
                } else {
                    std::cout << "\nThe NoC selected don't support virtual channels" << std::endl;
                    return -1;
                }
            }

            //------------- Binding StopSim -------------//
            u_STOP->i_TG_NUM_PACKETS_SENT[elementId](w_TG_NUM_PACKETS_SENT[elementId]);
            u_STOP->i_TG_NUM_PACKETS_RECEIVED[elementId](w_TG_NUM_PACKETS_RECEIVED[elementId]);
            u_STOP->i_TG_EOT[elementId](w_TG_EOT[elementId]);

        }
    }

    sc_trace_file *tf = NULL;
    if( TRACE ) {
        // TEMP - TODO: Mudar quando for integrar no RS
        //tf=sc_create_vcd_trace_file("system");
        tf=sc_create_vcd_trace_file(u_NOC->moduleName());
        // Signal tracing
        sc_trace(tf, w_CLK, "CLK");
        sc_trace(tf, w_RST, "RST");
        sc_trace(tf, w_EOS, "EOS");
        sc_trace(tf, w_GLOBAL_CLOCK, "GLOBAL_CLK");
        for( unsigned short x = 0; x < X_SIZE; x++ ) {
            for( unsigned short y = 0; y < Y_SIZE; y++ ) {
                // Get the router ID according X and Y network position to configure the system
                unsigned short rId = COORDINATE_TO_ID(x,y);

                // Assembling signal names to trace
                char strDataIn[20];
                sprintf(strDataIn,"L_DATA_IN_%u_%u",x,y);
                char strValIn[20];
                sprintf(strValIn,"L_VAL_IN_%u_%u",x,y);
                char strRetIn[20];
                sprintf(strRetIn,"L_RET_IN_%u_%u",x,y);
                char strDataOut[20];
                sprintf(strDataOut,"L_DATA_OUT_%u_%u",x,y);
                char strValOut[20];
                sprintf(strValOut,"L_VAL_OUT_%u_%u",x,y);
                char strRetOut[20];
                sprintf(strRetOut,"L_RET_OUT_%u_%u",x,y);
                char strTgEOT[15];
                sprintf(strTgEOT,"TG_%u_%u_EOT",x,y);
                if(NUM_VC > 1) {
                    char strVcSelIn[25];
                    sprintf(strVcSelIn,"L_VC_SEL_IN_%u_%u",x,y);
                    char strVcSelOut[26];
                    sprintf(strVcSelOut,"L_VC_SEL_OUT_%u_%u",x,y);
                    for(unsigned short vcBit = 0; vcBit < vcWidth; vcBit++) {
                        char strVcSelInBit[29];
                        sprintf(strVcSelInBit,"%s(%u)",strVcSelIn,vcBit);
                        char strVcSelOutBit[30];
                        sprintf(strVcSelOutBit,"%s(%u)",strVcSelOut,vcBit);

                        sc_trace(tf,w_IN_VC_SEL[rId][vcBit],strVcSelInBit);
                        sc_trace(tf,w_OUT_VC_SEL[rId][vcBit],strVcSelOutBit);
                    }
                }

                // Add to trace
                sc_trace(tf,w_IN_DATA[rId],strDataIn);
                sc_trace(tf,w_IN_VALID[rId],strValIn);
                sc_trace(tf,w_IN_RETURN[rId],strRetIn);
                sc_trace(tf,w_OUT_DATA[rId],strDataOut);
                sc_trace(tf,w_OUT_VALID[rId],strValOut);
                sc_trace(tf,w_OUT_RETURN[rId],strRetOut);
                sc_trace(tf,w_TG_EOT[rId],strTgEOT);
            }
        }
    }

    std::cout << "\n\n\n//////////////////////////////////////////////" << std::endl;
    std::cout << "////////////// SoCIN Simulator  //////////////" << std::endl;
    std::cout << "////////////// Start Simulation //////////////" << std::endl;
    std::cout << "//////////////////////////////////////////////" << std::endl << std::endl << std::endl;

    // Start the simulation (the StopSim will stop it with sc_stop())
    time_t start;
    time_t finish;
    time(&start);
    sc_start();
    time(&finish);

    double execTime = difftime(finish,start);
    char* formattedTime = print_time((unsigned long long) execTime);

    printf("\n\nExecuted in: %s\n\n",formattedTime);

    if(TRACE) {
        sc_close_vcd_trace_file(tf);
        generateListNodesGtkwave();
    }


    // Deallocating simulator units and auxiliar data
    for( unsigned short i = 0; i < numElements; i++ ) {
        delete u_TMs[i];
    }
    delete[] formattedTime;
    delete PLUGIN_MANAGER;

    return 0;
}

/*!
 * \brief setupSimulator Setup all global definitions for the
 * simulator.
 *
 * Configure:
 * X_SIZE      : X dimension
 * Y_SIZE      : Y dimension
 * FLIT_WIDTH  : Width of data channel
 * TRACE       : Flag to indicate if must be generated a .vcd file (wavefor)
 * CLK_PERIOD  : Via command-line argument[1] - define the clock period. If argument missing, the default (1 ns) clock period would be used.
 * WORK_DIR    : Via command-line argument[2] - define the work output folder to report data files. If argument missing, the default ("./work") would be used.
 * PLUGINS_DIR : Via command-line argument[3] - define the plugins directory or use default ("./plugins") directory if argument would missing.
 *
 * Finally, try parse the configuration file and load plugins.
 * \param argc Number of arguments passed to executable.
 * \param argv A char array with the arguments passed to executable.
 * \return Zero if all configurations successfully performed. A positive number with the index of message error.
 */
unsigned int setupSimulator(int argc, char* argv[]) {
    // TODO: Fazer parse para obter configurações do sistema (X_SIZE, Y_SIZE, FLIT_WIDTH, TRACE...)
    X_SIZE = 3;
    Y_SIZE = 3;

    NUM_VC = 0;

    if( NUM_VC == 1 ) NUM_VC = 2; // 1 VC is not accepted by the model because vcWidth = ceil(log2(NUM_VC)) == 0

    FLIT_WIDTH = 34;
    TRACE = true;

    switch(argc) {
        case 4: // argv[3] == Plugins folder
            PLUGINS_DIR = argv[3];
        case 3: // argv[2] == Work folder
            WORK_DIR = argv[2];
        case 2: // argv[1] == Clock period
            CLK_PERIOD = atof(argv[1]);
            if(CLK_PERIOD <= 0) {
                CLK_PERIOD = 1;
            }
        case 1: // Only simulator filename
            break;
        default: break;
    }

    // Trying parsing configuration file and load plugins
    if( PLUGIN_MANAGER->parseFile() ) {
        if( PLUGIN_MANAGER->loadPlugins() ) {
            std::cout << "Plugins loaded!" << std::endl;
        } else {
            return 1; // Return 1 to main that show message 1
        }
    } else {
        return 2;    // Main show message 2 from the SETUP_MESSAGES[2] vector
    }

    return 0;
}

/*!
 * \brief generateListNodesGtkwave Generate the list_nodes.sav file
 * to be read by Gtkwave tool and load signals in pre-defined layout.
 */
void generateListNodesGtkwave() {

    FILE* out;
    char fileName[256];
    sprintf(fileName,"%s/list_nodes.sav",WORK_DIR);
    if ((out=fopen(fileName,"wb")) == NULL){
        printf("\n\tCannot open the file \"%s\" to write gtkwave list nodes.\n",fileName);
        return;
    }
    unsigned short x_size = X_SIZE;
    unsigned short y_size = Y_SIZE;
    unsigned short flit_width = FLIT_WIDTH;
    unsigned short vcWidth = ceil(log2(NUM_VC));
    unsigned short x,y,vcBit;   // Loop counters

    fprintf(out,"[timestart] 0");
    fprintf(out,"\n@200");
    fprintf(out,"\n-System-Signals");
    fprintf(out,"\n@22");
    fprintf(out,"\nSystemC.GLOBAL_CLK[31:0]");
    fprintf(out,"\n@28");
    fprintf(out,"\nSystemC.CLK");
    fprintf(out,"\nSystemC.RST");
    fprintf(out,"\n@200");
    fprintf(out,"\n-Core-Connections");

    for( x = 0; x < x_size; x++) {
        for( y = 0; y < y_size; y++) {
            fprintf(out,"\n-Core_%u_%u",x,y);
            // For virtual channel in selector
            if(NUM_VC > 1) {
                fprintf(out,"\n@c00028");
                fprintf(out,"\n#{SystemC.L_VC_SEL_IN_%u_%u([%u:0])}",x,y,vcWidth-1);
                for(vcBit = vcWidth-1; vcBit < vcWidth; vcBit--) {
                    fprintf(out," SystemC.L_VC_SEL_IN_%u_%u(%u)",x,y,vcBit);
                }
                fprintf(out,"\n@28");
                for(vcBit = vcWidth-1; vcBit < vcWidth; vcBit--) {
                    fprintf(out,"\nSystemC.L_VC_SEL_IN_%u_%u(%u)",x,y,vcBit);
                }
                fprintf(out,"\n@1401200");
                fprintf(out,"\n-group_end");
            }

            fprintf(out,"\n@28");
            fprintf(out,"\nSystemC.L_VAL_IN_%u_%u",x,y);
            fprintf(out,"\nSystemC.L_RET_IN_%u_%u",x,y);
            fprintf(out,"\n@22");
            fprintf(out,"\nSystemC.L_DATA_IN_%u_%u[%u:0]",x,y,flit_width-1);

            // For virtual channel out selector
            if(NUM_VC > 1) {
                fprintf(out,"\n@c00028");
                fprintf(out,"\n#{SystemC.L_VC_SEL_OUT_%u_%u([%u:0])}",x,y,vcWidth-1);
                for(vcBit = vcWidth-1; vcBit < vcWidth; vcBit--) {
                    fprintf(out," SystemC.L_VC_SEL_OUT_%u_%u(%u)",x,y,vcBit);
                }
                fprintf(out,"\n@28");
                for(vcBit = vcWidth-1; vcBit < vcWidth; vcBit--) {
                    fprintf(out,"\nSystemC.L_VC_SEL_OUT_%u_%u(%u)",x,y,vcBit);
                }
                fprintf(out,"\n@1401200");
                fprintf(out,"\n-group_end");
            }

            fprintf(out,"\n@28");
            fprintf(out,"\nSystemC.L_VAL_OUT_%u_%u",x,y);
            fprintf(out,"\nSystemC.L_RET_OUT_%u_%u",x,y);
            fprintf(out,"\n@22");
            fprintf(out,"\nSystemC.L_DATA_OUT_%u_%u[%u:0]",x,y,flit_width-1);
            fprintf(out,"\n@200");
        }
    }

    fprintf(out,"\n-Status");
    fprintf(out,"\n@28");
    for( x = 0; x < x_size; x++)
        for( y = 0; y < y_size; y++)
            fprintf(out,"\nSystemC.TG_%u_%u_EOT",x,y);

    fprintf(out,"\nSystemC.EOS");
    fprintf(out,"\n[pattern_trace] 1");
    fprintf(out,"\n[pattern_trace] 0\n");

    fclose(out);

}

////////////////////////////////////////////////////////////////////////////////
char *print_time(unsigned long long total_sec)
////////////////////////////////////////////////////////////////////////////////
{
    char* str = new char[256];
    unsigned int hours, mins, secs;

    hours = (unsigned int)( total_sec/3600);
    mins  = (unsigned int)((total_sec - hours*3600)/60);
    secs  = (unsigned int)( total_sec - hours*3600 - mins*60);

    if (hours) {
        sprintf(str," %d hour, %d min and %d sec ",hours, mins, secs);
    } else {
        if (mins) {
            sprintf(str," %d min and %d sec ",mins, secs);
        } else {
            sprintf(str," %d sec",secs);
        }
    }
    return str;
}
