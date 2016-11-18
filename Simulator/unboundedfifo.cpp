#include "unboundedfifo.h"

UnboundedFifo::UnboundedFifo(sc_module_name mn)
    : SoCINModule(mn),
      i_CLK("UnboundedFifo_i"),
      i_RST("UnboundedFifo_iRST"),
      i_DATA_IN("UnboundedFifo_iDATA_IN"),
      i_RD("UnboundedFifo_iREAD"),
      i_WR("UnboundedFifo_iWRITE"),
      o_RD_OK("UnboundedFifo_oREAD_OK"),
      o_WR_OK("UnboundedFifo_oWRITE_OK"),
      o_DATA_OUT("UnboundedFifo_oDATA_OUT")
{
    o_WR_OK.initialize(1);

    SC_METHOD(fifoProcess);
    sensitive << i_CLK.pos();
}

UnboundedFifo::~UnboundedFifo() {
    while(!m_FIFO.empty()) {
        m_FIFO.pop();
    }
}

void UnboundedFifo::fifoProcess() {

    if( i_RST.read() ) {
        o_RD_OK.write(0);
        o_WR_OK.write(1);
        o_DATA_OUT.write(0);
        while(!m_FIFO.empty()) {
            m_FIFO.pop();
        }
    } else {
        if( i_RD.read() == 1 ) {    // Read - only remove data from the FIFO if it isn't empty
            if(!m_FIFO.empty()) {
                m_FIFO.pop();
            }

            if( m_FIFO.empty() ) {  // if the fifo is empty after remove the data, then reset the outputs
                o_DATA_OUT.write(0);
                o_RD_OK.write(0);
            } else {                // if fifo isn't empty after remove the data, then update the outputs
                o_DATA_OUT.write( m_FIFO.front() );
                o_RD_OK.write(1);
            }
        }

        if( i_WR.read() == 1 ) {    // It writes in fifo
            m_FIFO.push( i_DATA_IN.read() );  // Puts on the queue
            o_DATA_OUT.write(m_FIFO.front()); // Update the output data if necessary
            o_RD_OK.write(1);                 // Enable FIFO-!EMPTY because of the write
        }

    }
}

