/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : FIFO
FILE   : Fifo.h
--------------------------------------------------------------------------------
DESCRIPTION: Implements a FIFO to be used as input buffer in ParIS router.
This implementation is based on an integration of the original VHDL codes of
fifo_datapath and fifo_controller into a single module.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 26/08/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/

#ifndef __FIFO_H__
#define __FIFO_H__

#include "../src/Memory.h"

/////////////////////////////////////////////////////////////
/// Concrete implementation of FIFO Memory
/////////////////////////////////////////////////////////////
/*!
 * \brief The FIFO class implements a FIFO to be used as
 *  buffer in ParIS router. This implementation is based on
 * an integration of the original VHDL codes of fifo_datapath
 * and fifo_controller into a single module.
 * This fifo uses a circular approach.
 */
class FIFO : public IMemory {
public:

    // Signals
    sc_signal<unsigned short>   r_CUR_STATE;          // Current FIFO state
    sc_signal<unsigned short>   w_NEXT_STATE;         // Next FIFO state
    sc_signal<unsigned short>   r_READ_POINTER;       // Read pointer
    sc_signal<unsigned short>   r_WRITE_POINTER;      // Write pointer
    sc_signal<unsigned short>   w_NEXT_READ_POINTER;  // Next read pointer
    sc_signal<unsigned short>   w_NEXT_WRITE_POINTER; // Next write pointer
    sc_vector<sc_signal<Flit> > r_FIFO;               // Buffer

    // Module's processes
    // Controller
    void p_NEXT_STATE();
    void p_CURRENT_STATE();

    // Datapath
    void p_NEXT_WRITE_POINTER();
    void p_NEXT_READ_POINTER();
    void p_POINTERS_REGISTERS();
    void p_WRITE_FIFO();
    void p_OUTPUTS();

    // No FIFO
    void p_NULL();

    // Debug
    void p_DEBUG_CONTROL();
    void p_DEBUG_DATAPATH();

    SC_HAS_PROCESS(FIFO);
    FIFO(sc_module_name mn,
         unsigned short memSize,
         unsigned short ROUTER_ID,
         unsigned short PORT_ID);

    inline ModuleType moduleType() const { return SoCINModule::TMemory; }
    inline const char* moduleName() const { return "FIFO"; }

    ~FIFO();
};
/////////////////////////////////////////////////////////////
/// END Concrete implementation of FIFO Memory
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

#endif // FIFO_H
