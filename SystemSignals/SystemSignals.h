/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : SystemSignals
FILE   : SystemSignals.h
--------------------------------------------------------------------------------
DESCRIPTION: Module to generate the global clock counter and reset signal
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 01/10/2016 - 1.0     - Eduardo Alves da Silva      | Initial implementation
--------------------------------------------------------------------------------
*/
#ifndef __SYSTEMSIGNALS_H__
#define __SYSTEMSIGNALS_H__

#include <systemc>

class SystemSignals : public ::sc_core::sc_module {
public:

    // INTERFACE
    // System signals
    ::sc_core::sc_in<bool>           i_CLK;
    ::sc_core::sc_out<bool>          o_RST;
    ::sc_core::sc_out<unsigned long long> o_GLOBAL_CLOCK;

    // Internal Signals
    unsigned long long r_COUNTER; // Global clock counter register

    // Module's processes
    void p_CLOCK();
    void p_RESET();

    SC_HAS_PROCESS(SystemSignals);
    SystemSignals(::sc_core::sc_module_name);
};

#endif // __SYSTEMSIGNALS_H__
