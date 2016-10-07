/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : StopSim
FILE   : StopSim.h
--------------------------------------------------------------------------------
DESCRIPTION: Module to control the stop of the simulation
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 01/10/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef __STOPSIM_H__
#define __STOPSIM_H__

#include "../SoCINModule.h"

/*!
 * \brief The StopSim class determines the stop of simulation.
 */
class StopSim : public SoCINModule {
protected:
    unsigned short numRouters;
public:
    // Interface
    // System signals
    sc_in<bool>          i_CLK;         // Clock
    sc_in<bool>          i_RST;         // Reset
    sc_out<bool>         o_EOS;         // End-of-Simulation
    sc_in<unsigned long> i_CLK_CYCLES;  // Global counter of cycles

    // Traffic Generators signals
    sc_vector<sc_in<bool> >         i_TG_EOT;                 // End-of-Transmission of each TG
    sc_vector<sc_in<unsigned int> > i_TG_NUM_PACKETS_SENT;    // Number of packets sent by each TG - MAX: 4Giga Packets (2^32)
    sc_vector<sc_in<unsigned int> > i_TG_NUM_PACKETS_RECEIVED;// Number of packets received by each TG

    // Signals
    sc_signal<unsigned long> r_TOTAL_PACKETS_SENT;    // Register - current packets sent
    sc_signal<unsigned long> r_TOTAL_PACKETS_RECEIVED;// Register - current packets received
    sc_signal<bool>          w_EOT;                   // End-of-Transmission

    // Internal data structures
    char* confFileName;

    // Module's process
    void p_STOP();

    SC_HAS_PROCESS(StopSim);
    StopSim(sc_module_name mn,
            unsigned short nRouters,
            char* filename);

    ModuleType moduleType() const { return SoCINModule::TStopSim; }
    const char* moduleName() const { return "StopSim"; }

    ~StopSim(){}
};

#endif // __STOPSIM_H__
