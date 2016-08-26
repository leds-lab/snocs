/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : IMemory
FILE   : Memory.h
--------------------------------------------------------------------------------
DESCRIPTION: It is the interface class for memory implementations.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 25/08/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef MEMORY_H
#define MEMORY_H

#include "../SoCINGlobal.h"

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Interface for Memory Units
/////////////////////////////////////////////////////////////
/*!
 * \brief The IMemory class is an interface (abstract class)
 * for memory units (buffers)
 */
class IMemory : public SoCINModule {
protected:
    unsigned short memSize;     // Memory capacity - unsigned short (16B) corresponds
                                // to 65.536 (2^16) maximum representation
public:
    // Interface - System
    sc_in<bool>                  i_CLK;      // Clock
    sc_in<bool>                  i_RST;      // Reset

    // FIFO interface
    sc_out<bool>                 o_READ_OK;  // FIFO has a data to be read (not empty)
    sc_out<bool>                 o_WRITE_OK; // FIFO has room to be written (not full)
    sc_in<bool>                  i_READ;     // Command to read a data from the FIFO
    sc_in<bool>                  i_WRITE;    // Command to write a data into the FIFO
    sc_in<sc_uint<FIFO_WIDTH> >  i_DATA;     // Input data channel
    sc_out<sc_uint<FIFO_WIDTH> > o_DATA;     // Output data channel

    unsigned short int XID, YID, PORT_ID;

    IMemory(sc_module_name mn,
            unsigned short int memSize,
            unsigned short int XID,
            unsigned short int YID,
            unsigned short int PORT_ID)
        : SoCINModule(mn),
          memSize(memSize),
          i_CLK("MEM_iCLK"),
          i_RST("MEM_iRST"),
          o_READ_OK("MEM_oREAD_OK"),
          o_WRITE_OK("MEM_oWRITE_OK"),
          i_READ("MEM_iREAD"),
          i_WRITE("MEM_iWRITE"),
          i_DATA("MEM_iDATA"),
          o_DATA("MEM_oDATA"),
          XID(XID),
          YID(YID),
          PORT_ID(PORT_ID) {}
    ~IMemory() = 0;
};
inline IMemory::~IMemory() {}
/////////////////////////////////////////////////////////////
/// END Interface for Memory Units
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Typedefs for Factory Methods of concrete Memory units
/////////////////////////////////////////////////////////////
/*!
 * \brief create_Memory Typedef for instantiate a Memory
 * \param sc_simcontext A pointer of simulation context (required for correct plugins use)
 * \param sc_module_name Name for the module to be instantiated
 * \param XID Column identifier of the router in the network
 * \param YID Row identifier of the router in the network
 * \param PORT_ID Port identifier of the router
 * \param memSize Capacity of the memory to be instantiated
 * // TODO: If memSize is 0 - no memory or unbounded memory????
 * \return A method for instantiate a Memory
 */
typedef IMemory* create_Memory(sc_simcontext*,
                               sc_module_name,
                               unsigned short int XID,
                               unsigned short int YID,
                               unsigned short int PORT_ID,
                               unsigned short int memSize);

/*!
 * \brief destroy_Memory Typedef for deallocating a
 * Memory unit
 */
typedef void destroy_Memory(IMemory*);
/////////////////////////////////////////////////////////////

#endif // MEMORY_H
