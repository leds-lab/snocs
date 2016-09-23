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
#include "../PluginManager/PluginManager.h"

/////////////////////////////////////////////////////////////////////////
/// Definitions
/////////////////////////////////////////////////////////////////////////
// Singleton instance
#define PARAMS Parameters::instance() // Get instance of parameters

// Network info
#define X_SIZE PARAMS->xSize                // Network X dimension
#define Y_SIZE PARAMS->ySize                // Network Y dimension
// Packet Format
#define FLIT_WIDTH PARAMS->wordWidth        // Width of the flit (dataWidth + framing)
#define RIB_WIDTH PARAMS->ribWidth          // Width of the addressing field (RIB) in the header

// Buffering
#define FIFO_IN_DEPTH PARAMS->fifoInDepth   // Input buffers depth
#define FIFO_OUT_DEPTH PARAMS->fifoOutDepth // Output buffers depth

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
    // Network info
    unsigned short xSize;
    unsigned short ySize;
    // Packet Format
    unsigned short wordWidth;
    unsigned short ribWidth;

    // Buffering
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
#define LOG2_N              2  	// Log2 of N

// FLOW CONTROL
//enum fsm_state {S0,S1,S2};     	// States of the FSMs of handshake-type ifc and ofc
//#define CREDIT              4 	// Number of credits at power up
#define CREDIT_COUNTER_SIZE 3 	// Size of the credit counter

// TRAFFIC CLASSES
#define RT0                 0
#define RT1                 1
#define NRT0                2
#define NRT1                3

// STAMPLER PARAMETERS (block used for aging in XIN_AG
enum stampler_fsm_state {STAMPLER_S0, STAMPLER_S1};
#define AGE_CLOCK_WIDTH     2 // VHDL Original name: SIZE_REF
#define AGE_WIDTH           3 // VHDL Original name: SIZE_COUNT

// PARIS CHANNEL TYPES
#define PARIS_CHANNEL_NULL    0
#define PARIS_CHANNEL_DEFAULT 1
#define PARIS_CHANNEL_CS      2
#define PARIS_CHANNEL_AG      3

// PARIS CHANNELS SELECTION
#define PARIS_VC0_TYPE      PARIS_CHANNEL_DEFAULT
#define PARIS_VC1_TYPE      PARIS_CHANNEL_NULL

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
#define LOCAL_ID  0
#define NORTH_ID  1
#define EAST_ID   2
#define SOUTH_ID  3
#define WEST_ID   4

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// DEBUG CONTROL ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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
