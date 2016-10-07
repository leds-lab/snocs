/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : No modules - global definitions
FILE   : Parameters.h
--------------------------------------------------------------------------------
DESCRIPTION: Global definitions for all the system
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 04/09/2016 - 1.0     - Eduardo Alves da Silva      | Initial implementation
--------------------------------------------------------------------------------
*/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "../export.h"

class PluginManager;

/////////////////////////////////////////////////////////////////////////
/// Definitions
/////////////////////////////////////////////////////////////////////////
// Singleton instance
#define PARAMS Parameters::instance() // Get instance of parameters

// System info
#define CLK_PERIOD PARAMS->clkPeriod        // Clock period
#define TRACE PARAMS->traceSystem           // Generate system waveform
#define WORK_DIR PARAMS->workDir            // Work folder
#define PLUGINS_DIR PARAMS->pluginsDir      // Plugins folder
#define CONF_FILE PARAMS->confFile          // File with simulator definitions
// Network info
#define X_SIZE PARAMS->xSize                // Network X dimension
#define Y_SIZE PARAMS->ySize                // Network Y dimension
/*!
  Packet Format for the Mesh 2D SoCIN
  e.g. 32-bit data width.
  The flit width (FLIT_WIDTH) is 34-bit because 2 bits are framing:
       EOP (Bit pos. 33) (end-of-packet - trailer) and BOP (Bit pos. 32) (begin-of-packet - header)
  HLP (Higher Level Protocol) - Described below packet representation.
  RIB (Routing Information Bits) - 8 bits - 4 for xDestination and 4 for yDestionation
                                   (Max. Mesh Size: 16x16 = 256 routers)
    Packet representation:
  |-----------------------------------------------------|
  | EOP | BOP |             Data[31..0]                 |
  |-----------------------------------------------------|
  |  0  |  1  | HLP | RIB.xdest[7..0] | RIB.ydest[3..0] | -> Header flit
  |-----------------------------------------------------|
  |  0  |  0  |            Payload (data)               | -> Payload flit
  |  0  |  0  |                  .                      | -> Payload flit
  |  0  |  0  |                  .                      | -> Payload flit
  |  0  |  0  |                  .                      | -> Payload flit
  |  0  |  0  |              Unlimited                  | -> Payload flit
  |-----------------------------------------------------|
  |  1  |  0  |            Last Payload                 | -> Trailer flit
  |-----------------------------------------------------|

  HLP is composed as follow: From bit 31 to 8 | HLP[31..8] (8..0 is RIB)

  HLP[31..30] Thread ID       - Max. 4 threads
  HLP[20..18] Traffic Class   - Max. 8 classes
  HLP[15..12] Packet X source - From(RIB_WIDTH*2-1) to ([RIB_WIDTH*2-RIB_WIDTH/2] or [RIB_WIDTH+RIB_WIDTH/2])
  HLP[11.. 8] Packet Y source - From(RIB_WIDTH+RIB_WIDTH/2-1) to(RIB_WIDTH)
     Packet source (both x and y) is used to generate simulation report or to some routing technique
     that consider the source address.

*/
#define FLIT_WIDTH PARAMS->wordWidth                        // Width of the flit (dataWidth + framing)
#define RIB_WIDTH PARAMS->ribWidth                          // Width of the addressing field (RIB) in the header
#define NUMBER_TRAFFIC_CLASSES PARAMS->numberOfClasses      // Number of traffic classes
#define TRAFFIC_CLASS_POSITION PARAMS->trafficClassPosition // Position of the traffic class in the header
#define NUMBER_OF_THREADS PARAMS->numberOfThreads           // Number of threads supported by the simulator
#define THREAD_ID_POSITION PARAMS->threadIdPosition         // Position of the thread id in the header
#define PACKET_TYPE_POSITION 16 // ???

// Buffering
#define FIFO_IN_DEPTH PARAMS->fifoInDepth   // Input buffers depth
#define FIFO_OUT_DEPTH PARAMS->fifoOutDepth // Output buffers depth
#define NUM_VC PARAMS->numVirtualChannels   // Number of virtual channels

// Flow Control
#define CREDIT FIFO_IN_DEPTH                // Number of credits at power up

// Plugin manager instance
#define PLUGIN_MANAGER PARAMS->pm           // Plugin manager

/////////////////////////////////////////////////////////////////////////
/// Parameters of the system
/////////////////////////////////////////////////////////////////////////
// Singleton
class EXP_DEFINES Parameters {
public:
    // Plugin Manager - (de)allocate instances from plugins
    PluginManager* pm;

    // Attributes
    // System info
    float clkPeriod;
    bool traceSystem;
    char* workDir;
    char* pluginsDir;
    char* confFile;
    // Network info
    unsigned short xSize;
    unsigned short ySize;
    // Packet Format
    unsigned short wordWidth;
    unsigned short ribWidth;

    unsigned short trafficClassPosition;
    unsigned short numberOfClasses;
    unsigned short threadIdPosition;
    unsigned short numberOfThreads;
    // Buffering
    unsigned short numVirtualChannels;
    unsigned short fifoInDepth;
    unsigned short fifoOutDepth;

private:
    // Singleton
    static Parameters* params;
    // Constructor - private
    Parameters();
    Parameters(const Parameters& ); // Copy
    // = operator
    Parameters& operator= (const Parameters&);
    // Destructor
    ~Parameters();
public:
    static Parameters* instance();
};


///////////////////////////////////////////////////////////
/// Global definitions
///////////////////////////////////////////////////////////


/////// OLD ///////

#define SIM // Used to isolate non-synthesizable code
////////////////////////////// ROUTER PARAMETERS ///////////////////////////////
// MECHANISMS TYPES
#define ROUTING_TYPE        0
#define FC_TYPE             1
#define ARBITER_TYPE        0
#define SWITCH_TYPE         0

// PACKET FORMAT
//#define FLIT_WIDTH          34 	// Width of the flit
//#define SIDEBAND_WIDTH      2 	// Width of the sideband field(framing bits)
//#define DATA_WIDTH          32 	// Width of the data field
//#define RIB_WIDTH           8 	// Width of the addressing field (RIB) in the header
#define FLIT_TYPE_WIDTH     2  	// Width of the header field that specifies the flit type
#define HEADER_LENGTH       1	// Length of the header
#define PAYLOAD_FRAME       0	// Frame for payload flits
#define HEADER_FRAME        1	// Frame for header  flits
#define TRAILER_FRAME       2	// Frame for trailer flits

// MASKS FOR EXTRACTION OF FIELDS FROM A PACKET
#define DATA_MASK           0xFFFFFFFF
#define X_SRC_MASK          0xF000
#define Y_SRC_MASK          0xF00
#define X_DEST_MASK         0xF0
#define Y_DEST_MASK         0xF

// BUFFERING
//#define FIFO_WIDTH          34 	// Width of the buffer words (= FLIT_WIDTH)
//#define FIFO_IN_DEPTH       4 	// Number of positions of the input buffers
#define FIFO_IN_LOG2_DEPTH  2 	// Log2 of INPUT_FIFO_DEPTH
//#define FIFO_OUT_DEPTH      0 	// Number of positions of the output buffers
#define FIFO_OUT_LOG2_DEPTH 0 	// Log2 of OUTPUT_FIFO_DEPTH

// ARBITRATION
//#define N                   4  	// Number of requests received by an arbiter
//#define LOG2_N              2  	// Log2 of N

// FLOW CONTROL
enum fsm_state {S0,S1,S2};     	// States of the FSMs of handshake-type ifc and ofc
//#define CREDIT              4 	// Number of credits at power up
#define CREDIT_COUNTER_SIZE 3 	// Size of the credit counter

// TRAFFIC CLASSES
//#define RT0                 0
//#define RT1                 1
//#define NRT0                2
//#define NRT1                3

// STAMPLER PARAMETERS (block used for aging in XIN_AG
//enum stampler_fsm_state {STAMPLER_S0, STAMPLER_S1};
//#define AGE_CLOCK_WIDTH     2 // VHDL Original name: SIZE_REF
//#define AGE_WIDTH           3 // VHDL Original name: SIZE_COUNT

// PARIS CHANNEL TYPES
//#define PARIS_CHANNEL_NULL    0
//#define PARIS_CHANNEL_DEFAULT 1
//#define PARIS_CHANNEL_CS      2
//#define PARIS_CHANNEL_AG      3

// PARIS CHANNELS SELECTION
//#define PARIS_VC0_TYPE      PARIS_CHANNEL_DEFAULT
//#define PARIS_VC1_TYPE      PARIS_CHANNEL_NULL

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// CONSTANTS ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// REQUEST ENCODING
//#define REQ_L    0x10
//#define REQ_N    0x08
//#define REQ_E    0x04
//#define REQ_S    0x02
//#define REQ_W    0x01
//#define REQ_NONE 0x00

// PORTS IDs
//#define LOCAL_ID  0
//#define NORTH_ID  1
//#define EAST_ID   2
//#define SOUTH_ID  3
//#define WEST_ID   4

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// DEBUG CONTROL ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// NEW
//#define DEBUG_FIFO_CONTROL
//#define DEBUG_FIFO_DATAPATH
//#define DEBUG_SOCIN
//#define DEBUG_IFC
//#define DEBUG_OFC
//#define DEBUG_PARIS
//#define WAVEFORM_PARIS
//#define DEBUG_SOCIN
//#define WAVEFORM_SOCIN



// OLD
//#define DEBUG_REQ_REG
//#define DEBUG_FIFO_IN_DATAPATH
//#define DEBUG_FIFO_IN_CONTROLLER
//#define DEBUG_IFC_HANDSHAKE
//#define DEBUG_IFC_CREDIT
//#define DEBUG_FIFO_OUT_DATAPATH
//#define DEBUG_FIFO_OUT_CONTROLLER
//#define DEBUG_PG
//#define DEBUG_PPE
//#define DEBUG_OFC_HANDSHAKE
//#define DEBUG_OFC_CREDIT
//#define DEBUG_PARIS_INTERCONNECT
//#define DEBUG_STOPSIM
//#define DEBUG_STAMPLER
//#define DEBUG_STAMP_CHECKER
//#define DEBUG_CLASS_CHECKER

#endif // PARAMETERS_H
