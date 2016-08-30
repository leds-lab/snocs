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
 * parameterizable multiplexeres of templated data
 */
template<class DATA_TYPE>
class IMultiplexer : public SoCINModule {
protected:
    unsigned short numPorts;
public:

    // Interface
    sc_vector<sc_in<bool> >      i_SEL;      // Selector
    sc_vector<sc_in<DATA_TYPE> > i_DATA;     // Inputs
    sc_out<DATA_TYPE>            o_DATA;     // Output

    IMultiplexer(sc_module_name mn, unsigned short nPorts)
        : SoCINModule(mn),
          numPorts(nPorts),
          i_SEL("IMux_iSEL"),
          i_DATA("IMux_iDATA",nPorts),
          o_DATA("IMux_oDATA") {}

    ModuleType moduleType() const { return SoCINModule::Switch; }

    ~IMultiplexer();
};
template<class DATA_TYPE>
inline IMultiplexer<DATA_TYPE>::~IMultiplexer() {}
/////////////////////////////////////////////////////////////
/// Multiplexer one-hot encoding
/////////////////////////////////////////////////////////////
/*!
 * \brief The OneHotMux class is an implementation of a
 * parameterizable multiplexer using one-hot encoding
 * for template data type.
 * The implementation is on header file because the template usage
 */
template<class DATA_TYPE>
class OneHotMux : public IMultiplexer<DATA_TYPE> {
public:
    // Module's process
    void p_OUTPUT() {
        unsigned short i;       // Loop iterator
        for( i = 0; i < this->numPorts; i++ ) {
            if( this->i_SEL[i].read() == 1 ) {
                break;
            }
        }
        if( i < this->numPorts ) {
            this->o_DATA.write( this->i_DATA[i].read() );
        } else {
            this->o_DATA.write( 0 );
        }
    }

    SC_HAS_PROCESS(OneHotMux);
    OneHotMux(sc_module_name mn, unsigned short numPorts)
        : IMultiplexer<DATA_TYPE>(mn,numPorts)
    {
        this->i_SEL.init(numPorts);
        SC_METHOD(p_OUTPUT);
        for( unsigned short i = 0; i < numPorts; i++ ) {
            this->sensitive << this->i_SEL[i] << this->i_DATA[i];
        }
    }

    const char* moduleName() const { return "OneHotMux"; }

    ~OneHotMux() {}
};
/////////////////////////////////////////////////////////////
/// END Multiplexer one-hot
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Multiplexer binary
/////////////////////////////////////////////////////////////
/*!
 * \brief The BinaryMux class is an implementation of a
 * parameterizable multiplexer using binary encoding
 * for template data type
 */
template<class DATA_TYPE>
class BinaryMux : public IMultiplexer<DATA_TYPE> {
protected:
    unsigned short selSize;
public:

    // Module's process
    void p_OUTPUT() {
        unsigned short i;       // Loop iterator
        unsigned short sel = 0; // Input selected
        for( i = this->selSize-1; i != 0; i-- ) {
            sel = (sel << 1) | this->i_SEL[i].read();
        }
        sel = (sel << 1) | this->i_SEL[0].read();
        this->o_DATA.write( this->i_DATA[sel].read() );
    }

    SC_HAS_PROCESS(BinaryMux);
    BinaryMux(sc_module_name mn, unsigned short numPorts)
        : IMultiplexer<DATA_TYPE>(mn,numPorts)
    {
        selSize = (unsigned short) log2(numPorts);
        this->i_SEL.init( selSize );
        SC_METHOD(p_OUTPUT);
        for( unsigned short i = 0; i < selSize; i++ ) {
            this->sensitive << this->i_SEL[i];
        }
        for( unsigned short i = 0; i < numPorts; i++ ) {
            this->sensitive << this->i_DATA[i];
        }
    }

//    inline ModuleType moduleType() const { return SoCINModule::Switch; }
    inline const char* moduleName() const { return "BinarySignalMux"; }

    ~BinaryMux() {}
};
/////////////////////////////////////////////////////////////
/// END Multiplexer binary
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Testbench
/////////////////////////////////////////////////////////////
template<class DATA_TYPE>
class MultiplexerTestbench : public sc_module {
protected:
    unsigned short numPorts;
    unsigned short selSize;
public:
    sc_in<bool> i_CLK; // Clock

    sc_vector<sc_signal<bool> >      w_SEL;      // Selector
    sc_vector<sc_signal<DATA_TYPE> > w_DATA_IN;  // Inputs
    sc_signal<DATA_TYPE>             w_DATA_OUT; // Output

    // DUT - Design Under Test
    IMultiplexer<DATA_TYPE>* u_SWITCH;

    // Trace file
    sc_trace_file* tf;

    void p_STIMULUS();

    SC_HAS_PROCESS(MultiplexerTestbench);
    MultiplexerTestbench(sc_module_name mn, IMultiplexer<DATA_TYPE>* mux, unsigned short nPorts);

    ~MultiplexerTestbench();
};

#endif // MULTIPLEXERS_H
