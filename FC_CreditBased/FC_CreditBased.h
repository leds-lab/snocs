/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : IFC_CreditBased - OFC_CreditBased
FILE   : FC_CreditBased.h
--------------------------------------------------------------------------------
DESCRIPTION: They are the classes that implement the credit-based protocol of
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
#ifndef __FC_CREDITBASED_H__
#define __FC_CREDITBASED_H__

#include "../FlowControl/FlowControl.h"

/////////////////////////////////////////////////////////////
/// Concrete implementation of Input Flow Controller
/// Credit-based protocol
/////////////////////////////////////////////////////////////
/*!
 * \brief The IFC_CreditBased class implements the Input Flow Controller
 * of credit-based protocol
 */
class IFC_CreditBased : public IInputFlowControl {
public:

    // Module's processes
    void p_RETURN();
    void p_VALID();

    // Debug
    void p_DEBUG();

    // Packet monitoring - only to traffic meter use
    void p_ALERT_PACKET_RECEIVE();

    SC_HAS_PROCESS(IFC_CreditBased);
    IFC_CreditBased(sc_module_name mn,
                    unsigned short XID,
                    unsigned short YID,
                    unsigned short PORT_ID);

    inline ModuleType moduleType() const { return SoCINModule::TInputFlowControl; }
    inline const char* moduleName() const { return "IFC_CreditBased"; }
};
/////////////////////////////////////////////////////////////
/// END Concrete implementation of Input Flow Controller
/// Credit-based protocol
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Concrete implementation of Output Flow Controller
/// Credit-based protocol
/////////////////////////////////////////////////////////////
/*!
 * \brief The OFC_CreditBased class implements the Output Flow Controller
 * of credit-based protocol
 */
class OFC_CreditBased : public IOutputFlowControl {
protected:
    unsigned short numCredits; // Number of credits at power up
public:

    // Internal signals
    sc_signal<unsigned short> r_COUNTER; // The credit counter - 16 bits (short) -> Buffer depth max = 2^15 = count up 32,768 (unsigned short)

    // Module's processes
    void p_COUNTER();
    void p_OUTPUTS();

    // Debug
    void p_DEBUG();

    SC_HAS_PROCESS(OFC_CreditBased);
    OFC_CreditBased(sc_module_name mn,
                    unsigned short numCredits,
                    unsigned short XID,
                    unsigned short YID,
                    unsigned short PORT_ID);

    inline ModuleType moduleType() const { return SoCINModule::TOutputFlowControl; }
    inline const char* moduleName() const { return "OFC_CreditBased"; }
};
/////////////////////////////////////////////////////////////
/// END Concrete implementation of Output Flow Controller
/// Credit-based protocol
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////


#endif // __FC_CREDITBASED_H__
