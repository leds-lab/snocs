#include "../NoC/NoC.h"
#include "../StopSim/StopSim.h"
#include "../SystemSignals/SystemSignals.h"
#include "../TrafficMeter/TrafficMeter.h"

#include "../PluginManager/PluginManager.h"

// TEMP
#include "TerminalInstrumentation.h"

// SystemC
#include <systemc>

// STL
#include <ctime>

#define SNOCS_MAJOR 1
#define SNOCS_MINOR 0
#define SNOCS_PATCH 0

#define VERSION STRINGIFY( SNOCS_MAJOR.SNOCS_MINOR.SNOCS_PATCH )

using namespace sc_core;
using namespace sc_dt;

class InputParser;

// Forward declaration of functions below the main
unsigned int setupSimulator(int argc, char* argv[],InputParser& opt);
void generateListNodesGtkwave(unsigned short numElements);
char *print_time(unsigned long long total_sec);
void printConfiguration();

// Messages to setup of the simulator
const char* SETUP_MESSAGES[] =
{"Setup successfully performed!", // [0]
 "Fail on load plugins ;-(",      // [1]
 "Conf file parse fail - oops"    // [2]
};

void showHelp() {

    std::cout << "\nSoCIN Simulator Help" << std::endl;
#if defined(VERSION)
    std::cout << "Version: " << VERSION << std::endl;
#endif
    std::cout << "Developed by LEDS - Univali" << std::endl;
    std::cout << "Copyright 2017 LEDS - Univali. All rights reserved."
              << std::endl << std::endl << std::endl;

    std::cout << "SNoCS is the SoCIN Network-on-chip Simulator.\n"
                 "SoCIN (System-on-Chip Interconnection Network) is\n"
                 "a Network-on-Chip originally developed to 2D Mesh topology\n"
                 "using the ParIS router.\n"
                 "\nFor more information about SoCIN, please read it article.\n"
                 "\"ParIS: a parameterizable interconnect switch for networks-on-chip.\"\n"
                 "Available in IEEE Xplore: http://ieeexplore.ieee.org/document/1360570/"
              << std::endl << std::endl;

    std::cout << "This simulator was developed to extend the original SoCIN Simulator.\n"
                 "The SNoCS is a RTL-based implementation of ParIS router in SystemC.\n"
                 "This simulator was designed to comprise a set of network topologies\n"
                 "besides the 2D Mesh with alternative implementations to the basic\n"
                 "attributes of the network (routing, flow control, arbitration,\n"
                 "buffering, ...)." << std::endl;
    std::cout << "\nThe system is composed of a Network and terminal instrumentation\n"
                 "as described by Dally and Towles in:\n"
                 "\"Principles and Practices of Interconnection Networks\", 2004."
              << std::endl << std::endl << std::endl;

    std::cout << " >>> Usage: SNoCS TClk WORK_DIR PLUGINS_DIR [options]"
              << std::endl << std::endl;

    std::cout << " * TClk        : clock period in nanoseconds to system operation.\n"
                 " * WORK_DIR    : Directory to output simulation log files.\n"
                 "                 Must be an existing directory.\n"
                 " * PLUGINS_DIR : Directory with the plugins of the simulator.\n"
                 "                 Windows(.dll), Linux(.so) and OS X(.dylib)."
              << std::endl << std::endl;

    std::cout << "Options: " << std::endl
              << "   * To all options there are a default value                    *" << std::endl
              << "   * Not all options are used depending the system configuration *" << std::endl
              << "   * There are limits values (i.e. min and max) to the options   *" << std::endl;
    std::cout << "  -xsize value        Network X dimension. 2 =< Value =< (16 for 2D and 8 for 3D)" << std::endl
              << "                      Default=4, Min: 2, Max: [16 (2D) | 8 (3D)]" << std::endl
              << "  -ysize value        Network Y dimension. 2 =< Value =< (16 for 2D and 8 for 3D)" << std::endl
              << "                      Default=4, Min: 2, Max: [16 (2D) | 8 (3D)]" << std::endl
              << "  -zsize value        Network Z dimension. 1 < Value <= 4" << std::endl
              << "                      Default=0 (no Z dimension), Min: 2, Max: 4" << std::endl
              << "  -datawidth value    Number of bits of the data channel. Value >= 32" << std::endl
              << "                      Default=32, Min: 32, Max: 510" << std::endl
              << "  -fifoin value       Routers input buffers depth (flits). 1 < Value <= 1024." << std::endl
              << "                      Default=4, Min: 2, Max: 1024" << std::endl
              << "  -fifoout value      Routers output buffers depth (flits). 0 <= Value <= 1024." << std::endl
              << "                      Default=0 (no output buffers), Min: 0, Max: 1024" << std::endl
              << "  -vc value           Number of virtual channels. 0 <= Value <= 32" << std::endl
              << "                      Default=0 (no virtual channels), Min: 0, Max: 32" << std::endl
              << "  -trace              Generate waveforms." << std::endl
              << "                      Default= Don't generate waveforms" << std::endl;
    std::cout << "\nIMPORTANT: <xsize> and <ysize> options define the system size\n"
                 "(i.e. number of elements). Not only for 2D orthogonal topologies,\n"
                 "but also to some others, like ring (xsize * ysize = NumberOfElements).\n"
                 "The option <zsize> must be used only for 3D topologies!" << std::endl;

}

/// @author Eduardo
/// @brief Class to record and parse command-line arguments
class InputParser{
    public:
        InputParser (int &argc, char **argv){
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }
        /// @author Eduardo
        const std::string getCmdOption(const std::string &option) const{
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                std::string token = *itr;
                if( token[0] == '-' ) {
                    return "";
                }
                return *itr;
            }
            return "";
        }
        /// @author Eduardo
        bool cmdOptionExists(const std::string &option) const{
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }
    private:
        std::vector <std::string> tokens;
};

int sc_main(int argc, char* argv[]) {

    //// [1] Configuration
    InputParser optParser(argc,argv);
    if(optParser.cmdOptionExists("-h")
            || optParser.cmdOptionExists("--help")
            || optParser.cmdOptionExists("-help")) {
        showHelp();
        return 0;
    }

    unsigned int setupCode = setupSimulator(argc,argv,optParser);

    if( setupCode >= sizeof(SETUP_MESSAGES) / sizeof(SETUP_MESSAGES[0]) ) {
        std::cout << "Error on setup - message undefined." << std::endl;
    } else {
        std::cout << SETUP_MESSAGES[setupCode] << std::endl;
    }

    if( setupCode != 0) {
        std::cout << "\n\nFor info: SNoCS -h" << std::endl;
        return -1;
    }

    printConfiguration();

    /// [2] Network models building
    INoC* u_NOC = PLUGIN_MANAGER->nocInstance("NoC");

    if( u_NOC == NULL ) {
        delete PLUGIN_MANAGER;
        return -1;
    }

    // ------------------- Establishing system -------------------

    /// [3] Signals instantation
    // System signals
    sc_clock                 w_CLK("CLK", CLK_PERIOD, SC_NS); // System clock | Tclk=1 ns
    sc_signal<bool>          w_RST;                           // Reset
    sc_signal<unsigned long> w_GLOBAL_CLOCK;                  // Number of cycles

    unsigned short numElements = u_NOC->getNumberOfInterfaces();

    std::cout << " -- > Number of Elements: " << numElements << std::endl;

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

    /// [4] System models building and binding
    //////////////////////////////////////////////////////////////////////////////
    SystemSignals *u_SYS_SIGNALS = new SystemSignals("SystemSignals");
    //////////////////////////////////////////////////////////////////////////////
    u_SYS_SIGNALS->i_CLK(w_CLK);
    u_SYS_SIGNALS->o_RST(w_RST);
    u_SYS_SIGNALS->o_GLOBAL_CLOCK(w_GLOBAL_CLOCK);

    //////////////////////////////////////////////////////////////////////////////
    StopSim* u_STOP = new StopSim("StopSim",numElements,(char *)"stopsim");
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
    for( unsigned short elementId = 0; elementId < numElements; elementId++ ) {
        // Assembling TG name
        char strTgName[10];
        sprintf(strTgName,"TG_%u",elementId);
        // Instantiate TG
        TerminalInstrumentation* u_TG = new TerminalInstrumentation(strTgName,elementId,u_NOC->topologyType());

        // Assembling TM name
        char strTmName[10];
        sprintf(strTmName,"TM_%u",elementId);
        // Assembling out filename
        char* outFilename = new char[20];
        sprintf(outFilename,"ext_%u_out",elementId);
        // Instantiate TM
        TrafficMeter* u_TM = new TrafficMeter(strTmName,WORK_DIR,outFilename,u_NOC->topologyType(),true);
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

    /// [5] Trace generation
    sc_trace_file *tf = NULL;
    if( TRACE ) {
        char strWaveformFile[256];
        sprintf(strWaveformFile,"%s/snocs_wave",WORK_DIR);
        tf=sc_create_vcd_trace_file(strWaveformFile);
        // Signal tracing
        sc_trace(tf, w_CLK, "CLK");
        sc_trace(tf, w_RST, "RST");
        sc_trace(tf, w_EOS, "EOS");
        sc_trace(tf, w_GLOBAL_CLOCK, "GLOBAL_CLK");
        for( unsigned short elementId = 0; elementId < numElements; elementId++ ) {
            // Assembling signal names to trace
            char strDataIn[20];
            sprintf(strDataIn,"L_DATA_IN_%u",elementId);
            char strValIn[20];
            sprintf(strValIn,"L_VAL_IN_%u",elementId);
            char strRetIn[20];
            sprintf(strRetIn,"L_RET_IN_%u",elementId);
            char strDataOut[20];
            sprintf(strDataOut,"L_DATA_OUT_%u",elementId);
            char strValOut[20];
            sprintf(strValOut,"L_VAL_OUT_%u",elementId);
            char strRetOut[20];
            sprintf(strRetOut,"L_RET_OUT_%u",elementId);
            char strTgEOT[15];
            sprintf(strTgEOT,"TG_%u_EOT",elementId);
            if(NUM_VC > 1) {
                char strVcSelIn[25];
                sprintf(strVcSelIn,"L_VC_SEL_IN_%u",elementId);
                char strVcSelOut[26];
                sprintf(strVcSelOut,"L_VC_SEL_OUT_%u",elementId);
                for(unsigned short vcBit = 0; vcBit < vcWidth; vcBit++) {
                    char strVcSelInBit[29];
                    sprintf(strVcSelInBit,"%s(%u)",strVcSelIn,vcBit);
                    char strVcSelOutBit[30];
                    sprintf(strVcSelOutBit,"%s(%u)",strVcSelOut,vcBit);

                    sc_trace(tf,w_IN_VC_SEL[elementId][vcBit],strVcSelInBit);
                    sc_trace(tf,w_OUT_VC_SEL[elementId][vcBit],strVcSelOutBit);
                }
            }

            // Add to trace
            sc_trace(tf,w_IN_DATA[elementId],strDataIn);
            sc_trace(tf,w_IN_VALID[elementId],strValIn);
            sc_trace(tf,w_IN_RETURN[elementId],strRetIn);
            sc_trace(tf,w_OUT_DATA[elementId],strDataOut);
            sc_trace(tf,w_OUT_VALID[elementId],strValOut);
            sc_trace(tf,w_OUT_RETURN[elementId],strRetOut);
            sc_trace(tf,w_TG_EOT[elementId],strTgEOT);
        }
    }

    /// [7] System simulation
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

    /// [8] System destroying
    if(TRACE) {
        sc_close_vcd_trace_file(tf);
        generateListNodesGtkwave(numElements);
    }


    // Deallocating simulator units and auxiliar data
    for( unsigned short i = 0; i < numElements; i++ ) {
        delete u_TMs[i];
    }
    delete[] formattedTime;
    delete PLUGIN_MANAGER;

    return 0;
}

void printConfiguration() {

    std::cout << "  --- Configuration ---" << std::endl << std::endl;

    const char* prefix = {"  * "};

    std::cout << prefix << "tClk: " << CLK_PERIOD << " ns\n" \
              << prefix << "work_dir: " << WORK_DIR << "\n" \
              << prefix << "plugins_dir: " << PLUGINS_DIR << std::endl;

    std::cout << prefix << "X Size: " << X_SIZE << std::endl
              << prefix << "Y Size: " << Y_SIZE << std::endl;
    if( Z_SIZE > 1 ) {
        std::cout << prefix << "Z Size: " << Z_SIZE << std::endl;
    }
    std::cout << prefix << "Data Width: " << (FLIT_WIDTH-2) << std::endl;
    if( NUM_VC > 1 ) {
        std::cout << prefix << "Number of Virtual Channels: " << NUM_VC << std::endl;
    } else {
        std::cout << prefix << "No virtual channels!" << std::endl;
    }

    std::cout << prefix << "Routers input buffers depth (flits): " << FIFO_IN_DEPTH << std::endl;
    if( FIFO_OUT_DEPTH > 0 ) {
        std::cout << prefix << "Routers output buffers depth (flits): " << FIFO_OUT_DEPTH << std::endl;
    } else {
        std::cout << prefix << "No output buffers in the routers!" << std::endl;
    }

    if( TRACE ) {
        std::cout << prefix << "Generate Waveforms (.vcd)!" << std::endl;
    } else {
        std::cout << prefix << "Don't generate Waveforms (.vcd)!" << std::endl;
    }

}

int getIntArg(InputParser& opt,std::string arg, int defaultValue, int min, int max = 0) {
    if( opt.cmdOptionExists(arg) ) {
        std::string var = opt.getCmdOption(arg);
        if( var.empty() ) {
            std::cout << arg << ": Argument missing or wrong... using default = "<< defaultValue << std::endl;
            return defaultValue;
        } else {
            int varValue = atoi(var.c_str());
            if(varValue < min) {
                std::cout << arg << ": Value not permitted. The minimum is \'" << min
                          << "\'... using default = " << defaultValue  << std::endl;
                return defaultValue;
            } else if(varValue > max && max > 0) {
                std::cout << arg << ": Value not permitted. The maximum is \'" << max
                          << "\'... using default = " << defaultValue  << std::endl;
                return defaultValue;
            } else {
                return varValue;
            }
        }
    } else {
        return defaultValue;
    }
}

/*!
 * \brief setupSimulator Setup all global definitions for the
 * simulator.
 *
 * Configure:
 * Clock period; Work folder; Plugins folder; System Dimension;
 * trace enable/disable
 *
 * Finally, try parse the configuration file and load plugins.
 * \param argc Number of arguments passed to executable.
 * \param argv A char array with the arguments passed to executable.
 * \param opt A object to parse command-line arguments
 * \return Zero if all configurations successfully performed. A positive number with the index of message error.
 */
unsigned int setupSimulator(int argc, char* argv[], InputParser &opt) {

    bool clkDefault = true;
    bool workDefault = true;
    bool pluginsDefault = true;

    std::cout << "Simulator <tClk_in_ns> <work_dir> <plugins_dir>" << std::endl;
    if(argc > 1) {
        switch(argc) {
            case 4:
                pluginsDefault = false;
            case 3:
                workDefault = false;
            case 2:
                clkDefault = false;
                break;
            default:
                pluginsDefault = false;
                workDefault = false;
                clkDefault = false;
                break;
        }
    } else {
        std::cout << "Using default values!" << std::endl;
    }

    if( !clkDefault ) {
        CLK_PERIOD = atof(argv[1]);
        if( CLK_PERIOD <= 0 ) {
            CLK_PERIOD = 1;
        }
    }
    if( !workDefault ) {
        WORK_DIR = argv[2];
    }
    if( !pluginsDefault ) {
        PLUGINS_DIR = argv[3];
    }

    Z_SIZE = getIntArg(opt,"-zsize",0,2,4);
    int max2Daxis = 16;
    if(Z_SIZE > 0) {
        max2Daxis = 8;
    }
    X_SIZE = getIntArg(opt,"-xsize",4,2,max2Daxis);
    Y_SIZE = getIntArg(opt,"-ysize",4,2,max2Daxis);
    FLIT_WIDTH = getIntArg(opt,"-datawidth",32,32,510) + 2; // Data Width + 2-bit framming (EOP & BOP)
    NUM_VC = getIntArg(opt,"-vc",0,0,32);
    FIFO_IN_DEPTH = getIntArg(opt,"-fifoin",4,2,1024);
    FIFO_OUT_DEPTH = getIntArg(opt,"-fifoout",0,0,1024);

    if( opt.cmdOptionExists("-trace") ) {
        TRACE = true;
    } else {
        TRACE = false;
    }

    if( NUM_VC == 1 ) {
        std::cout << "Changing the number of virtual channels to 2,\n"
                     "because 1 is the same as no virtual channels!" << std::endl;
        NUM_VC = 2; // 1 VC is not accepted by the model because vcWidth = ceil(log2(NUM_VC)) == 0
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
void generateListNodesGtkwave(unsigned short numElements) {

    FILE* out;
    char fileName[512];
    sprintf(fileName,"%s/list_nodes.sav",WORK_DIR);
    if ((out=fopen(fileName,"wb")) == NULL){
        printf("\n\tCannot open the file \"%s\" to write gtkwave list nodes.\n",fileName);
        return;
    }
    unsigned short flit_width = FLIT_WIDTH;
    unsigned short vcWidth = ceil(log2(NUM_VC));
    unsigned short elementId,vcBit;   // Loop counters

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

    for( elementId = 0; elementId < numElements; elementId++) {
        fprintf(out,"\n-Core_%u",elementId);
        // For virtual channel in selector
        if(NUM_VC > 1) {
            fprintf(out,"\n@c00028");
            fprintf(out,"\n#{SystemC.L_VC_SEL_IN_%u([%u:0])}",elementId,vcWidth-1);
            for(vcBit = vcWidth-1; vcBit < vcWidth; vcBit--) {
                fprintf(out," SystemC.L_VC_SEL_IN_%u(%u)",elementId,vcBit);
            }
            fprintf(out,"\n@28");
            for(vcBit = vcWidth-1; vcBit < vcWidth; vcBit--) {
                fprintf(out,"\nSystemC.L_VC_SEL_IN_%u(%u)",elementId,vcBit);
            }
            fprintf(out,"\n@1401200");
            fprintf(out,"\n-group_end");
        }

        fprintf(out,"\n@28");
        fprintf(out,"\nSystemC.L_VAL_IN_%u",elementId);
        fprintf(out,"\nSystemC.L_RET_IN_%u",elementId);
        fprintf(out,"\n@22");
        fprintf(out,"\nSystemC.L_DATA_IN_%u[%u:0]",elementId,flit_width-1);

        // For virtual channel out selector
        if(NUM_VC > 1) {
            fprintf(out,"\n@c00028");
            fprintf(out,"\n#{SystemC.L_VC_SEL_OUT_%u([%u:0])}",elementId,vcWidth-1);
            for(vcBit = vcWidth-1; vcBit < vcWidth; vcBit--) {
                fprintf(out," SystemC.L_VC_SEL_OUT_%u(%u)",elementId,vcBit);
            }
            fprintf(out,"\n@28");
            for(vcBit = vcWidth-1; vcBit < vcWidth; vcBit--) {
                fprintf(out,"\nSystemC.L_VC_SEL_OUT_%u(%u)",elementId,vcBit);
            }
            fprintf(out,"\n@1401200");
            fprintf(out,"\n-group_end");
        }

        fprintf(out,"\n@28");
        fprintf(out,"\nSystemC.L_VAL_OUT_%u",elementId);
        fprintf(out,"\nSystemC.L_RET_OUT_%u",elementId);
        fprintf(out,"\n@22");
        fprintf(out,"\nSystemC.L_DATA_OUT_%u[%u:0]",elementId,flit_width-1);
        fprintf(out,"\n@200");
    }

    fprintf(out,"\n-Status");
    fprintf(out,"\n@28");
    for( elementId = 0; elementId < numElements; elementId++)
        fprintf(out,"\nSystemC.TG_%u_EOT",elementId);

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
