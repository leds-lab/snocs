/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : TrafficMeter
FILE   : TrafficMeter.h
--------------------------------------------------------------------------------
DESCRIPTION: Traffic meter to attach on links between router or external interfaces.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 04/10/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef __TRAFFICMETER_H__
#define __TRAFFICMETER_H__

#include "../SoCINModule.h"
#include "../SoCINDefines.h"

/*!
 * \brief The TrafficMeter class implements a link traffic meter
 * that collects the data being transmitted and writes a log file
 * with all packets crossing in the link.
 */
class TrafficMeter : public SoCINModule {
protected:
    // Internal data structures
    char* workDir;          // Out folder - to put the log files
    char* outFileName;      // Filename of the log

    FILE* outFile;          // File of the log

    unsigned short trafficClassWidth;    // Width of the field traffic class in the header flit
    unsigned short threadIdWidth;        // Width of the fielad thread id in the header flit

    UIntVar packetHeader;
    unsigned long cycleOfArriving;

public:
    // Interface
    // System signals
    sc_in<bool>          i_CLK;        // Clock
    sc_in<bool>          i_RST;        // Reset
    sc_in<bool>          i_EOS;        // End-of-simulation - monitoring to close log files
    sc_in<unsigned long> i_CLK_CYCLES; // Global clock counter

    // Link signals
    sc_vector<sc_in<bool> > i_VC_SEL; // Virtual channel selector if the link has virtual channel
    sc_in<bool>             i_VALID;  // Valid status
    sc_in<bool>             i_RETURN; // Return status
    sc_in<Flit>             i_DATA;   // Data link

    // Module's processes
    void p_PROBE();
    void p_FINISH();

    // Aux. method to write a information
    void writeInfo();

    SC_HAS_PROCESS(TrafficMeter);
    TrafficMeter(sc_module_name mn,
                 char* workDir,
                 char* fileName);

    ModuleType moduleType() const { return SoCINModule::TTrafficMeter; }
    const char* moduleName() const { return "TrafficMeter"; }

    ~TrafficMeter();
};

#endif // __TRAFFICMETER_H__
