/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : IDemultiplexer - OneHotDemux - BinaryDemux
FILE   : Multiplexers.h
--------------------------------------------------------------------------------
DESCRIPTION: Demultiplexers interface and implementations to be used by the system
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 31/08/2016 - 1.0     - Eduardo Alves da Silva      | First implementation
--------------------------------------------------------------------------------
*/
#ifndef __DEMULTIPLEXERS_H__
#define __DEMULTIPLEXERS_H__

#include <systemc>
using namespace sc_core;

/////////////////////////////////////////////////////////////
/// Demultiplexer interface (abstract class)
/////////////////////////////////////////////////////////////
/*!
 * \brief The IDemultiplexer class is an interface for
 * parameterizable multiplexeres of templated data
 */
template<class DATA_TYPE>
class IDemultiplexer : public sc_module {
protected:
    unsigned short numPorts;
public:

    // Interface
    sc_vector<sc_in<bool> >       i_SEL;      // Selector
    sc_in<DATA_TYPE>              i_DATA;     // Input
    sc_vector<sc_out<DATA_TYPE> > o_DATA;     // Outputs

    IDemultiplexer(sc_module_name mn, unsigned short nPorts)
        : sc_module(mn),
          numPorts(nPorts),
          i_SEL("IDemux_iSEL"),
          i_DATA("IDemux_iDATA"),
          o_DATA("IDemux_oDATA",nPorts) {}

    ~IDemultiplexer() = 0;
};
template<class DATA_TYPE>
inline IDemultiplexer<DATA_TYPE>::~IDemultiplexer() {}
/////////////////////////////////////////////////////////////
/// END Demultiplexer interface
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Demultiplexer one-hot encoding
/////////////////////////////////////////////////////////////
/*!
 * \brief The OneHotDemux class is an implementation of a
 * parameterizable demultiplexer using one-hot encoding
 * for template data type.
 * The implementation is on header file because the template usage
 */
template<class DATA_TYPE>
class OneHotDemux : public IDemultiplexer<DATA_TYPE> {
public:
    // Module's process
    void p_OUTPUTS() {
        DATA_TYPE dNull = 0;
        unsigned short i;       // Loop iterator
        for( i = 0; i < this->numPorts; i++ ) {
            if( this->i_SEL[i].read() == 1 ) {
                this->o_DATA[i].write(this->i_DATA.read());
            } else {
                this->o_DATA[i].write(dNull);
            }
        }
    }

    SC_HAS_PROCESS(OneHotDemux);
    OneHotDemux(sc_module_name mn, unsigned short numPorts)
        : IDemultiplexer<DATA_TYPE>(mn,numPorts)
    {
        this->i_SEL.init(numPorts);
        SC_METHOD(p_OUTPUTS);
        this->sensitive << this->i_DATA;
        for( unsigned short i = 0; i < numPorts; i++ ) {
            this->sensitive << this->i_SEL[i];
        }
    }

    ~OneHotDemux() {}
};
/////////////////////////////////////////////////////////////
/// END Demultiplexer one-hot
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Demultiplexer binary
/////////////////////////////////////////////////////////////
/*!
 * \brief The BinaryDemux class is an implementation of a
 * parameterizable demultiplexer using binary encoding
 * for template data type
 */
template<class DATA_TYPE>
class BinaryDemux : public IDemultiplexer<DATA_TYPE> {
protected:
    unsigned short selSize;
public:

    // Module's process
    void p_OUTPUTS() {
        DATA_TYPE dNull = 0;
        unsigned short i;       // Loop iterator
        unsigned short sel = 0; // Output selected
        for( i = this->selSize-1; i != 0; i-- ) {
            sel = (sel << 1) | this->i_SEL[i].read();
        }
        sel = (sel << 1) | this->i_SEL[0].read();

        for( i = 0; i < this->numPorts; ++i ) {
            if( i == sel ) {
                this->o_DATA[i].write(this->i_DATA.read());
            } else {
                this->o_DATA[i].write(dNull);
            }
        }
    }

    SC_HAS_PROCESS(BinaryDemux);
    BinaryDemux(sc_module_name mn, unsigned short numPorts)
        : IDemultiplexer<DATA_TYPE>(mn,numPorts)
    {
        selSize = (unsigned short) ceil(log2(numPorts));
        this->i_SEL.init( selSize );
        SC_METHOD(p_OUTPUTS);
        this->sensitive << this->i_DATA;
        for( unsigned short i = 0; i < selSize; i++ ) {
            this->sensitive << this->i_SEL[i];
        }
    }

    ~BinaryDemux() {}
};
/////////////////////////////////////////////////////////////
/// END Multiplexer binary
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////


#endif // __DEMULTIPLEXERS_H__
