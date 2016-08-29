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

#ifndef FIFO_H
#define FIFO_H

#include "../Memory/Memory.h"

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
    sc_signal<unsigned short>        r_CUR_STATE;          // Current FIFO state
    sc_signal<unsigned short>        w_NEXT_STATE;         // Next FIFO state
    sc_signal<unsigned short>        r_READ_POINTER;       // Read pointer
    sc_signal<unsigned short>        r_WRITE_POINTER;      // Write pointer
    sc_signal<unsigned short>        w_NEXT_READ_POINTER;  // Next read pointer
    sc_signal<unsigned short>        w_NEXT_WRITE_POINTER; // Next write pointer
    sc_vector<sc_signal<sc_uint<FIFO_WIDTH> > > r_FIFO;    // Buffer

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

    SC_HAS_PROCESS(FIFO);
    FIFO(sc_module_name mn,
         unsigned short memSize,
         unsigned short XID,
         unsigned short YID,
         unsigned short PORT_ID);

    inline ModuleType moduleType() const { return SoCINModule::Memory; }
    inline const char* moduleName() const { return "FIFO"; }

    ~FIFO();
};
/////////////////////////////////////////////////////////////
/// END Concrete implementation of FIFO Memory
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * Factory method for instantation e deallocation of modules from the plugin
 */
extern "C" {
////////////////// FIFO Factory //////////////////
    SS_EXP IMemory* new_Memory(sc_simcontext* simcontext,
                               sc_module_name moduleName,
                               unsigned short int XID,
                               unsigned short int YID,
                               unsigned short int PORT_ID,
                               unsigned short int memSize) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new FIFO(moduleName,memSize,XID,YID,PORT_ID);
    }
    SS_EXP void delete_Memory(IMemory* mem) {
        delete mem;
    }
}

#endif // FIFO_H
