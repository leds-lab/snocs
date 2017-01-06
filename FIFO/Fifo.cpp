#include "Fifo.h"

//#define DEBUG_FIFO_CONTROL
//#define DEBUG_FIFO_DATAPATH

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief FIFO::FIFO Constructor of the memory that allocate the memory according size and
/// register the processes of the FIFO
/// \param mn Module name
/// \param memSize Memory size - capacity of the FIFO
/// \param ROUTER_ID Router identifier in the network
/// \param PORT_ID Port identifier in the router
FIFO::FIFO(sc_module_name mn,
           unsigned short memSize,
           unsigned short ROUTER_ID,
           unsigned short PORT_ID)
    : IMemory(mn,memSize,ROUTER_ID,PORT_ID),
      r_CUR_STATE("FIFO_rCUR_STATE"),
      w_NEXT_STATE("FIFO_wNEXT_STATE"),
      r_READ_POINTER("FIFO_rREAD_POINTER"),
      r_WRITE_POINTER("FIFO_rWRITE_POINTER"),
      w_NEXT_READ_POINTER("FIFO_wNEXT_READ_POINTER"),
      w_NEXT_WRITE_POINTER("FIFO_wNEXT_WRITE_POINTER"),
      r_FIFO("FIFO_rFIFO",memSize)
{

    if(memSize > 0) { // With FIFO
        // Registering processes
        // Controller
        SC_METHOD(p_NEXT_STATE);
        sensitive << i_WRITE << i_READ << r_CUR_STATE;

        SC_METHOD(p_CURRENT_STATE);
        sensitive << i_CLK.pos() << i_RST;

        // Datapath
        SC_METHOD(p_NEXT_WRITE_POINTER);
        sensitive << r_CUR_STATE << i_WRITE << r_WRITE_POINTER;

        SC_METHOD(p_NEXT_READ_POINTER);
        sensitive << r_CUR_STATE << i_READ << r_READ_POINTER;

        SC_METHOD(p_POINTERS_REGISTERS);
        sensitive << i_CLK.pos() << i_RST;

        SC_METHOD(p_WRITE_FIFO);
        sensitive << i_CLK.pos() << i_RST;

        SC_METHOD(p_OUTPUTS);
        sensitive << r_CUR_STATE << r_READ_POINTER;

    } else { // No FIFO
        SC_METHOD(p_NULL);
        sensitive << i_WRITE << i_READ << i_DATA;
    }
#ifdef DEBUG_FIFO_CONTROL
        SC_METHOD(p_DEBUG_CONTROL);
        sensitive << i_CLK.pos();
#endif
#ifdef DEBUG_FIFO_DATAPATH
        SC_METHOD(p_DEBUG_DATAPATH);
        sensitive << i_CLK.pos();
#endif
}
/*!
 * \brief FIFO::~FIFO Default destructor
 */
FIFO::~FIFO() {}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////// PROCESESSES /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/////////////////////////////// CONTROLLER /////////////////////////////////////
/*!
 * \brief FIFO::p_NEXT_STATE This process determines the next state of the FIFO
 * taking into account the current state and the write and read commands
 */
void FIFO::p_NEXT_STATE() {
    if( r_CUR_STATE.read() == 0 ) { // FIFO EMPTY
        if( i_WRITE.read() ) {
            w_NEXT_STATE.write( r_CUR_STATE.read() + 1 );
        } else {
            w_NEXT_STATE.write( r_CUR_STATE.read() );
        }
    } else {
        if( r_CUR_STATE.read() == memSize ) { // FIFO FULL
            if( i_READ.read() ) {
                w_NEXT_STATE.write( r_CUR_STATE.read() - 1 );
            } else {
                w_NEXT_STATE.write( r_CUR_STATE.read() );
            }
        } else { // FIFO NEITHER EMPTY, NEITHER FULL
            if( i_WRITE.read() == 1 ) {
                if( i_READ.read() ) {
                    w_NEXT_STATE.write( r_CUR_STATE.read() );       // rd & wr
                } else {
                    w_NEXT_STATE.write( r_CUR_STATE.read() + 1 );   // !rd & wr
                }
            } else {
                if( i_READ.read() ) {
                    w_NEXT_STATE.write( r_CUR_STATE.read() - 1 );   // rd & !wr
                } else {
                    w_NEXT_STATE.write( r_CUR_STATE.read() );       // !rd & !wr
                }
            }
        }
    }
}

/*!
 * \brief FIFO::p_CURRENT_STATE This process implements the register
 * that stores the state of the control FSM
 */
void FIFO::p_CURRENT_STATE() {
    if( i_RST.read() == 1 ) {
        r_CUR_STATE.write(0);
    } else {
        r_CUR_STATE.write( w_NEXT_STATE.read() );
    }
}

/////////////////////////////// DATAPATH /////////////////////////////////////
/*!
 * \brief FIFO::p_NEXT_WRITE_POINTER It determines the next write pointer,
 * by incrementing the current one when the FIFO is not full and there is
 * a writing into the FIFO
 */
void FIFO::p_NEXT_WRITE_POINTER() {
    if( (r_CUR_STATE.read() != memSize) && (i_WRITE.read() == 1) ) {
        if( r_WRITE_POINTER.read() == (memSize-1) ) {
            w_NEXT_WRITE_POINTER.write(0);
        } else {
            w_NEXT_WRITE_POINTER.write( r_WRITE_POINTER.read() + 1 );
        }
    } else {
        w_NEXT_WRITE_POINTER.write( r_WRITE_POINTER.read() );
    }
}

/*!
 * \brief FIFO::p_NEXT_READ_POINTER It determines the next read pointer,
 * by decrementing the current one when the FIFO is not empty and
 * there is a reading from the FIFO.
 */
void FIFO::p_NEXT_READ_POINTER() {
    if( (r_CUR_STATE.read() != 0) && (i_READ.read() == 1) ) {
        if( r_READ_POINTER.read() == (memSize - 1) ) {
            w_NEXT_READ_POINTER.write( 0 );
        } else {
            w_NEXT_READ_POINTER.write( r_READ_POINTER.read() + 1 );
        }
    } else {
        w_NEXT_READ_POINTER.write( r_READ_POINTER.read() );
    }
}

/*!
 * \brief FIFO::p_POINTERS_REGISTERS It implements the pointer registers
 */
void FIFO::p_POINTERS_REGISTERS() {
    if( i_RST.read() == 1 ) {
        r_WRITE_POINTER.write(0);
        r_READ_POINTER.write(0);
    } else {
        r_WRITE_POINTER.write( w_NEXT_WRITE_POINTER.read() );
        r_READ_POINTER.write( w_NEXT_READ_POINTER.read() );
    }
}

/*!
 * \brief FIFO::p_WRITE_FIFO It implements the FIFO memory
 */
void FIFO::p_WRITE_FIFO() {

    if( (i_WRITE.read()) && (r_CUR_STATE.read() != memSize) ) {
        for (unsigned short index = 0; index < memSize; index++) {
            if (index == r_WRITE_POINTER.read()) {
                r_FIFO[index].write( i_DATA.read() );
            } else {
                r_FIFO[index].write( r_FIFO[index].read() );
            }
        }
    }

    // The code above is a VHDL2SystemC mapping from the original VHDL code, but
    // it could be implemented just like below:

//    if( (i_WRITE.read()) && (r_CUR_STATE.read() < memSize) ) {
//        r_FIFO[r_WRITE_POINTER.read()].write( i_DATA.read() );
//    }
}

/*!
 * \brief FIFO::p_OUTPUTS It updates tje outputs
 */
void FIFO::p_OUTPUTS() {
    o_DATA.write( r_FIFO[r_READ_POINTER.read()].read() );

    if( r_CUR_STATE.read() == 0 ) { // FIFO empty
        o_READ_OK.write(0);
        o_WRITE_OK.write(1);
    } else {
        if( r_CUR_STATE.read() == memSize ) { // FIFO full
            o_READ_OK.write(1);
            o_WRITE_OK.write(0);
        } else { // Neither empty, neither full
            o_READ_OK.write(1);
            o_WRITE_OK.write(1);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// NO FIFO /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief FIFO::p_NULL Without fifo - direct connection
 */
void FIFO::p_NULL() {

    o_READ_OK.write( i_WRITE.read() );
    o_WRITE_OK.write( i_READ.read() );
    o_DATA.write( i_DATA.read() );
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// DEBUG //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void FIFO::p_DEBUG_CONTROL() {

    std::cout << "\n[FIFO] - Router[" << ROUTER_ID << "]-Port[" << PORT_ID << "] @ " << sc_time_stamp();
    std::cout << " NEXT_STATE= " << w_NEXT_STATE.read() \
              << " CURRENT_STATE= " << r_CUR_STATE.read();

}

void FIFO::p_DEBUG_DATAPATH() {
    std::cout << std::endl << name() << " @ " << sc_time_stamp();
    std::cout << " CURRENT_STATE= "<< r_CUR_STATE.read();

    std::cout << " FIFO[" << (memSize-1) << "..0]=";
    Flit f;
    for( unsigned int i = 0; i < memSize; i++ ) {
        f = r_FIFO[i].read();
        std::cout << " D[" << i << "]: " << f.data.to_string(SC_HEX_US,false);
    }

    std::cout << "  WR= " << i_WRITE.read();
    std::cout << "  RD= " << i_READ.read();
    std::cout << "  RD_PTR= " << r_READ_POINTER.read();
    std::cout << "  WR_PTR= " << r_WRITE_POINTER.read();
}

////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * Factory method for instantation e deallocation of modules from the plugin
 */
extern "C" {
////////////////// FIFO Factory //////////////////
    SS_EXP IMemory* new_Memory(sc_simcontext* simcontext,
                               sc_module_name moduleName,
                               unsigned short int ROUTER_ID,
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

        return new FIFO(moduleName,memSize,ROUTER_ID,PORT_ID);
    }
    SS_EXP void delete_Memory(IMemory* mem) {
        delete mem;
    }
}
