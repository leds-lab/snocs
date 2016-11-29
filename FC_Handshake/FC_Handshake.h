/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : IFC_Handshake - OFC_Handshake
FILE   : FC_Handshake.h
--------------------------------------------------------------------------------
DESCRIPTION: They are the classes that implement the handshake protocol of
flow control. The classes are implemented with plugins approach
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 25/08/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef __FC_HANDSHAKE_H__
#define __FC_HANDSHAKE_H__

#include "../FlowControl/FlowControl.h"

enum handshake_states {s_S0, s_S1, s_S2}; // States of the FSMs of handshake-type ifc and ofc

/////////////////////////////////////////////////////////////
/// Concrete implementation of Input Flow Controller
/// Handshake protocol
/////////////////////////////////////////////////////////////
/*!
 * \brief The IFC_Handshake class implements the Input Flow Controller
 * of handshake protocol
 */
class IFC_Handshake : public IInputFlowControl {
public:

    // Internal signals
    sc_signal<handshake_states> r_CUR_STATE;  // Current state of the handshake FSM
    sc_signal<handshake_states> w_NEXT_STATE; // Next state of the handshake FSM

    // Module's processes
    void p_CURRENT_STATE();
    void p_NEXT_STATE();
    void p_OUTPUTS();

    // Packet monitoring - only to traffic meter use
    void p_ALERT_PACKET_RECEIVE();

    unsigned short numberOfCyclesPerFlit() const { return 4; } // Handshake forward a flit in the link in 4 cycles

    SC_HAS_PROCESS(IFC_Handshake);
    IFC_Handshake(sc_module_name mn,
                  unsigned short ROUTER_ID,
                  unsigned short PORT_ID);

    inline ModuleType moduleType() const { return SoCINModule::TInputFlowControl; }
    inline const char* moduleName() const { return "IFC_Handshake"; }
};
/////////////////////////////////////////////////////////////
/// END Concrete implementation of Input Flow Controller
/// Handshake protocol
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Concrete implementation of Output Flow Controller
/// Handshake protocol
/////////////////////////////////////////////////////////////
/*!
 * \brief The OFC_Handshake class implements the Output Flow Controller
 * of handshake protocol
 */
class OFC_Handshake : public IOutputFlowControl {
public:

    // Internal signals
    sc_signal<handshake_states> r_CUR_STATE;  // Current state of the handshake FSM
    sc_signal<handshake_states> w_NEXT_STATE; // Next state of the handshake FSM

    // Module's processes
    void p_CURRENT_STATE();
    void p_NEXT_STATE();
    void p_OUTPUTS();

    SC_HAS_PROCESS(OFC_Handshake);
    OFC_Handshake(sc_module_name mn,
                  unsigned short ROUTER_ID,
                  unsigned short PORT_ID);

    inline ModuleType moduleType() const { return SoCINModule::TOutputFlowControl; }
    inline const char* moduleName() const { return "OFC_Handshake"; }
};
/////////////////////////////////////////////////////////////
/// END Concrete implementation of Output Flow Controller
/// Handshake protocol
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////


#endif // FC_HANDSHAKE_H
