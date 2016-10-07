/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : No modules - only data structures
FILE   : SoCINDefines.h
--------------------------------------------------------------------------------
DESCRIPTION: Data structures of transmission
             Data type
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 04/09/2016 - 1.0     - Eduardo Alves da Silva      | Initial implementation
--------------------------------------------------------------------------------
*/

#ifndef SOCINDEFINES_H
#define SOCINDEFINES_H

#include <systemc>
#include "Parameters/Parameters.h"

using namespace sc_core;
using namespace sc_dt;

/////////////////////////////////////////////////////////////////////////
/// Parameterizable data representation (unsigned)
/////////////////////////////////////////////////////////////////////////
class UIntVar : public sc_unsigned {
public:
    ////////// Constructors //////////
    UIntVar() : sc_unsigned(FLIT_WIDTH) {} // Default
    UIntVar(int v, int width)
    : sc_unsigned( width )
    { *this = v; }

    // Follow SystemC specs
    UIntVar( const UIntVar& v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( const sc_unsigned& v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( const sc_signed& v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( const char* v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( int64 v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( uint64 v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( long v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( unsigned long v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( int v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( unsigned int v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( double v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( const sc_bv_base& v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    UIntVar( const sc_lv_base& v )
    : sc_unsigned( FLIT_WIDTH )
    { *this = v; }

    ////////// Assignment operators //////////
    UIntVar& operator = ( const UIntVar& v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( const sc_unsigned& v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( const sc_signed& v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( const char* v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( int64 v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( uint64 v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( long v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( unsigned long v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( int v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( unsigned int v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( double v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( const sc_bv_base& v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( const sc_lv_base& v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( const sc_int_base& v )
    { sc_unsigned::operator = ( v ); return *this; }

    UIntVar& operator = ( const sc_uint_base& v )
    { sc_unsigned::operator = ( v ); return *this; }

    ~UIntVar() {}
};

/////////////////////////////////////////////////////////////////////////
/// Parameterizable data representation (signed)
/////////////////////////////////////////////////////////////////////////
class IntVar : public sc_signed {
public:
    ////////// Constructors //////////
    IntVar() : sc_signed(FLIT_WIDTH) {} // Default
    IntVar(int v, int width)
    : sc_signed( width )
    { *this = v; }

    // Follow SystemC specs
    IntVar( const IntVar& v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( const sc_unsigned& v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( const sc_signed& v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( const char* v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( int64 v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( uint64 v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( long v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( unsigned long v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( int v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( unsigned int v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( double v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( const sc_bv_base& v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    IntVar( const sc_lv_base& v )
    : sc_signed( FLIT_WIDTH )
    { *this = v; }

    ////////// Assignment operators //////////
    IntVar& operator = ( const IntVar& v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( const sc_unsigned& v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( const sc_signed& v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( const char* v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( int64 v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( uint64 v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( long v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( unsigned long v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( int v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( unsigned int v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( double v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( const sc_bv_base& v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( const sc_lv_base& v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( const sc_int_base& v )
    { sc_signed::operator = ( v ); return *this; }

    IntVar& operator = ( const sc_uint_base& v )
    { sc_signed::operator = ( v ); return *this; }

    ~IntVar() {}
};
/////////////////////////////////////////////////////////////////////////
/// END of Parameterizable data representation
/////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
/// Packet structure
/////////////////////////////////////////////////////////////////////////
struct Packet {
    unsigned long int packetId;            // Packet identifier in the network - exclusive by packet
    unsigned short payloadLength;          // Number of payload flits
    float requiredBW;                      // Required bandwidth for the packet
    unsigned long int deadline;            // Defined deadline for the packet
    unsigned long int packetCreationCycle; // Packet cycle generation
};
/////////////////////////////////////////////////////////////////////////
/// END of Packet structure
/////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
/// Data type used in the communication channels
/////////////////////////////////////////////////////////////////////////
class Flit {
public:
    UIntVar data;         // Real data
    Packet* packet_ptr;   // Pointer to packet of this flit

    // Constructors
    Flit() : data(0), packet_ptr(NULL) {} // Default
    Flit(UIntVar value, Packet* packet) : data(value), packet_ptr(packet) {}     // Auxiliar
    Flit(const Flit& f) { this->data = f.data; this->packet_ptr = f.packet_ptr;} // Copy
    Flit(Flit& f)       { this->data = f.data; this->packet_ptr = f.packet_ptr;} // Copy
    Flit(int data)      { this->data = data; this->packet_ptr = NULL;}

    Flit& operator= (const Flit& flit)
    { this->data = flit.data; this->packet_ptr = flit.packet_ptr; return *this;}

    Flit& operator = (int data)
    { this->data  = data; this->packet_ptr = NULL; return *this;}

    bool operator== (const Flit& flit) const
    { return ( this->data == flit.data && this->packet_ptr == flit.packet_ptr ); }

    virtual ~Flit() {}

    friend std::ostream& operator<<(std::ostream& os, const Flit& flit)
    {
        os << "{" << std::endl << "Data:" << flit.data << ","
           << "Packet_ID:" << flit.packet_ptr << std::endl
           << "}";
        return os;
    }

    friend void sc_trace(::sc_core::sc_trace_file* tf, const Flit& flit, const std::string& nm)
    { sc_trace( tf, flit.data, nm ); }

};
/////////////////////////////////////////////////////////////////////////
/// END of Data type used in the communication channels
/////////////////////////////////////////////////////////////////////////


#endif // SOCINDEFINES_H
