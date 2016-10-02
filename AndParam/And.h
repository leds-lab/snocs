/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : And
FILE   : And.h
--------------------------------------------------------------------------------
DESCRIPTION: A parameterizable 1-bit and port
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 31/08/2016 - 1.0     - Eduardo Alves da Silva      | First implementation
--------------------------------------------------------------------------------
*/
#ifndef __AND_H__
#define __AND_H__

#include <systemc>

/*!
 * \brief The And class a parameterizable 1-bit and port
 */
class And : public ::sc_core::sc_module {
private:
    unsigned short numPorts;
public:
    // Interface
    ::sc_core::sc_vector< ::sc_core::sc_in<bool> > i_DATA;
    ::sc_core::sc_out<bool> o_DATA;

    // Module's process
    void p_AND() {
        bool v_AND = true;
        unsigned short i;
        for( i = 0; i < numPorts; i++) {
            v_AND &= i_DATA[i].read();
        }
        o_DATA.write(v_AND);
    }

    SC_HAS_PROCESS(And);
    And(::sc_core::sc_module_name mn,unsigned short nPorts)
        : sc_module(mn), numPorts(nPorts),
          i_DATA("And_iDATA",nPorts),
          o_DATA("And_oDATA")
    {
        SC_METHOD(p_AND);
        for( unsigned short i = 0; i < nPorts; i++ ) {
            sensitive << i_DATA[i];
        }
    }
};

#endif // __AND_H__
