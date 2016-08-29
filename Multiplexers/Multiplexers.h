/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : OneHotMultiplexer - BinaryMultiplexer
FILE   : Multiplexers.h
--------------------------------------------------------------------------------
DESCRIPTION: Multiplexers implementations to be used by the system
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 29/08/2016 - 1.0     - Eduardo Alves da Silva      | First implementation
--------------------------------------------------------------------------------
*/
#ifndef MULTIPLEXERS_H
#define MULTIPLEXERS_H

#include "../SoCINGlobal.h"

/////////////////////////////////////////////////////////////
/// Multiplexer interface (abstract class)
/////////////////////////////////////////////////////////////
/*!
 * \brief The IMultiplexer class is an interface of for
 * parameterizable multiplexeres using with 1-bit data width
 */
class IMultiplexer : public SoCINModule {
protected:
    unsigned short numPorts;
public:
    enum Encoding { OneHot, Binary };
    // Interface
    sc_vector<sc_in<bool> > i_SEL;      // Selector
    sc_vector<sc_in<bool> > i_DATA;     // Inputs
    sc_out<bool>            o_DATA;     // Output

    IMultiplexer(sc_module_name mn, unsigned short nPorts)
        : SoCINModule(mn),
          numPorts(nPorts),
          i_SEL("IMux_iSEL"),
          i_DATA("IMux_iDATA",nPorts),
          o_DATA("IMux_oDATA") {}

    ~IMultiplexer();
};
inline IMultiplexer::~IMultiplexer() {}
/////////////////////////////////////////////////////////////
/// Multiplexer one-hot encoding to 1-bit data
/////////////////////////////////////////////////////////////
/*!
 * \brief The OneHotSignalMux class is an implementation of a
 * parameterizable multiplexer using one-hot encoding
 * for 1-bit data width
 */
class OneHotSignalMux : public IMultiplexer {
public:
    // Module's process
    void p_OUTPUT();

    SC_HAS_PROCESS(OneHotSignalMux);
    OneHotSignalMux(sc_module_name, unsigned short numPorts);

    inline ModuleType moduleType() const { return SoCINModule::Switch; }
    inline const char* moduleName() const { return "OneHotSignalMux"; }

    ~OneHotSignalMux();
};
/////////////////////////////////////////////////////////////
/// END Multiplexer one-hot to 1-bit data
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Multiplexer binary to 1-bit data
/////////////////////////////////////////////////////////////
/*!
 * \brief The BinarySignalMux class is an implementation of a
 * parameterizable multiplexer using binary encoding
 * for 1-bit data width
 */
class BinarySignalMux : public IMultiplexer {
protected:
    unsigned short selSize;
public:

    // Module's process
    void p_OUTPUT();

    SC_HAS_PROCESS(BinarySignalMux);
    BinarySignalMux(sc_module_name mn, unsigned short numPorts);

    inline ModuleType moduleType() const { return SoCINModule::Switch; }
    inline const char* moduleName() const { return "BinarySignalMux"; }

    ~BinarySignalMux();
};
/////////////////////////////////////////////////////////////
/// END Multiplexer binary to 1-bit data
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Testbench
/////////////////////////////////////////////////////////////
class MultiplexerTestbench : public sc_module {
protected:
    unsigned short numPorts;
    unsigned short selSize;
public:
    sc_in<bool> i_CLK; // Clock

    sc_vector<sc_signal<bool> > w_SEL;      // Selector
    sc_vector<sc_signal<bool> > w_DATA_IN;  // Inputs
    sc_signal<bool>             w_DATA_OUT; // Output

    // DUT - Design Under Test
    IMultiplexer* u_SWITCH;

    // Trace file
    sc_trace_file* tf;

    void p_STIMULUS();

    SC_HAS_PROCESS(MultiplexerTestbench);
    MultiplexerTestbench(sc_module_name mn, IMultiplexer::Encoding enc, unsigned short nPorts);

    ~MultiplexerTestbench();
};

#endif // MULTIPLEXERS_H
