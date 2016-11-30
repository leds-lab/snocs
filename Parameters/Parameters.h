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
#define Z_SIZE PARAMS->zSize                // Network Z dimension (3D topologies)
// Convert from router cartesian coordinate in the MESH network to router ID
#define COORDINATE_2D_TO_ID(x,y) (y * X_SIZE + x)
#define COORDINATE_3D_TO_ID(x,y,z) ( COORDINATE_2D_TO_ID(x,y) + (X_SIZE * Y_SIZE * z) )
// Convert from network router ID to cartesian coordinate in the MESH
#define ID_TO_COORDINATE_2D_X(id) (id % X_SIZE)
#define ID_TO_COORDINATE_2D_Y(id) (id / X_SIZE)
#define ID_TO_COORDINATE_3D_Z(id) (id / (X_SIZE * Y_SIZE))
#define ID_TO_COORDINATE_3D_X(id) ( ID_TO_COORDINATE_2D_X(id - (X_SIZE*Y_SIZE*ID_TO_COORDINATE_3D_Z(id))) )
#define ID_TO_COORDINATE_3D_Y(id) ( ID_TO_COORDINATE_2D_Y(id - (X_SIZE*Y_SIZE*ID_TO_COORDINATE_3D_Z(id))) )
/*!
  TODO: IMPORTANT> Update
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
  |  0  |  1  | HLP | RIB.xdest[7..4] | RIB.ydest[3..0] | -> Header flit
  |-----------------------------------------------------|
  |  0  |  0  |            Payload (data)               | -> Payload flit
  |  0  |  0  |                  .                      | -> Payload flit
  |  0  |  0  |                  .                      | -> Payload flit
  |  0  |  0  |                  .                      | -> Payload flit
  |  0  |  0  |              Unlimited                  | -> Payload flit
  |-----------------------------------------------------|
  |  1  |  0  |            Last Payload                 | -> Trailer flit
  |-----------------------------------------------------|

  HLP is composed as follow: From bit 31 to 8 | HLP[31..8] (7..0 is RIB)

  HLP[31..30] Thread ID       - Max. 4 threads
  HLP[29..21] Free
  HLP[20..18] Traffic Class   - Max. 8 classes
  HLP[17..16] Packet type: 0x0 (NORMAL), 0x1 (ALOCATE CIRCUIT), 0x2 (RELEASE CIRCUIT), 0x3 (GRANT)
  HLP[15..12] Packet X source - From(RIB_WIDTH*2-1) to ([RIB_WIDTH*2-RIB_WIDTH/2] or [RIB_WIDTH+RIB_WIDTH/2])
  HLP[11.. 8] Packet Y source - From(RIB_WIDTH+RIB_WIDTH/2-1) to(RIB_WIDTH)
     Packet source (both x and y) is used to generate simulation report or to some routing technique
     that consider the source address.

     NOTE: The header format is used in topologies: Bus, Crossbar, Ring, Chordar-ring, Mesh2D, Torus2D.
           For Bus, Crossbar, Ring and Chordal-ring, the functions COORDINATE_2D_TO_ID and ID_TO_COORIDNATE_2D_[X|Y] are used to conversions.

  HEADER in 3D topology
  |----------------------------------------------------------------------|
  | EOP | BOP |                     Data[31..0]                          |
  |----------------------------------------------------------------------|
  |  0  |  1  | HLP | RIB.xdest[9..6] | RIB.ydest[5..2]| RIB.zdest[1..0] | -> Header flit.
  |----------------------------------------------------------------------|
    NOTE: X and Y are shifted to left and Z field is added for until 4 layers

*/
#define FLIT_WIDTH PARAMS->wordWidth           // Width of the flit (dataWidth + framing)
#define RIB_WIDTH PARAMS->ribWidth             // Width of the addressing field (RIB) in the header
#define N_CLASSES PARAMS->numberOfClasses      // Number of traffic classes
#define CLS_POS PARAMS->trafficClassPosition   // Position of the traffic class in the header
#define CMD_POSITION PARAMS->commandPosition   // Position of the command in the header
#define FID_POS 25

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
    unsigned long pckId;
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
    unsigned short zSize;
    // Packet Format
    unsigned short wordWidth;
    unsigned short ribWidth;

    unsigned short trafficClassPosition;
    unsigned short numberOfClasses;
    unsigned short commandPosition;
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

#define STRINGIFY( Arg ) x_str(Arg)
#define x_str(Arg) #Arg


////////////////////////////////////////////////////////////////////////////////
///////////////////////////// DEBUG CONTROL ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//#define DEBUG_FIFO_CONTROL
//#define DEBUG_FIFO_DATAPATH
//#define DEBUG_SOCIN
//#define DEBUG_IFC
//#define DEBUG_OFC
//#define DEBUG_PARIS
//#define WAVEFORM_PARIS
//#define DEBUG_SOCIN
//#define WAVEFORM_SOCIN

#endif // PARAMETERS_H
