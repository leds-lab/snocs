#ifndef __UNBOUNDEDFIFO_H__
#define __UNBOUNDEDFIFO_H__

#include "../SoCINModule.h"
#include "../SoCINDefines.h"

#include <queue>

class UnboundedFifo : public SoCINModule {
public:
    // System signals
    sc_in<bool>      i_CLK;     // Clock
    sc_in<bool>      i_RST;     // Reset
    // FIFO interface
    sc_in<Flit>      i_DATA_IN; // Input data channel
    sc_in<bool>      i_RD;      // Command to read a data from the FIFO
    sc_in<bool>      i_WR;      // Command to write a data into FIFO
    sc_out<bool>     o_RD_OK;   // FIFO has a data to be read (not empty)
    sc_out<bool>     o_WR_OK;   // FIFO has room to be written (not full)
    sc_out<Flit>     o_DATA_OUT;// Output data channel

    std::queue<Flit> m_FIFO; // The FIFO

    // Module's processes
    void fifoProcess();

    SC_HAS_PROCESS(UnboundedFifo);
    UnboundedFifo(sc_module_name mn);

    ModuleType moduleType() const { return SoCINModule::TMemory; }
    const char* moduleName() const { return "UnboundedFifo"; }

    ~UnboundedFifo();
};


#endif // __UNBOUNDEDFIFO_H__
