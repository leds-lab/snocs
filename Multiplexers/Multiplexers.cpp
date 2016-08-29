#include "Multiplexers.h"


OneHotSignalMux::OneHotSignalMux(sc_module_name mn, unsigned short nPorts)
    : IMultiplexer(mn,nPorts)
{
    i_SEL.init(nPorts);

    SC_METHOD(p_OUTPUT);
    for( unsigned short i = 0; i < nPorts; i++ ) {
        sensitive << i_SEL[i] << i_DATA[i];
    }

}

OneHotSignalMux::~OneHotSignalMux(){}

void OneHotSignalMux::p_OUTPUT() {

    unsigned short i;       // Loop iterator

    for( i = 0; i < numPorts; i++ ) {
        if( i_SEL[i].read() == 1 ) {
            break;
        }
    }

    if( i < numPorts ) {
        std::cout << "Selected[" << i << "] @ " << sc_time_stamp() << std::endl;
        o_DATA.write( i_DATA[i].read() );
    } else {
        std::cout << "Not Selected @" << sc_time_stamp() << std::endl;
        o_DATA.write( 0 );
    }

}


////////////////////////////////////////////////////////////////////////////////////////////


BinarySignalMux::BinarySignalMux(sc_module_name mn, unsigned short nPorts)
    : IMultiplexer(mn,nPorts),
      numPorts(nPorts)
{
    selSize = (unsigned short) log2(nPorts);

    i_SEL.init( selSize );

    std::cout << "Mux - NPorts: " << nPorts << ", Sel Bits: " << selSize << std::endl;

    SC_METHOD(p_OUTPUT);
    for( unsigned short i = 0; i < selSize; i++ ) {
        sensitive << i_SEL[i];
    }
    for( unsigned short i = 0; i < nPorts; i++ ) {
        sensitive << i_DATA[i];
    }

}

BinarySignalMux::~BinarySignalMux(){}

void BinarySignalMux::p_OUTPUT() {

    unsigned short i;       // Loop iterator
    unsigned short sel = 0; // Input selected

    for( i = selSize; i != 0; i-- ) {
        sel = (sel << 1) | i_SEL[i].read();
    }

    std::cout << "Selected: " << sel << std::endl;
    o_DATA.write( i_DATA[sel].read() );

}

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Testbench
/////////////////////////////////////////////////////////////
MultiplexerTestbench::MultiplexerTestbench(sc_module_name mn,
                                           IMultiplexer::Encoding enc,
                                           unsigned short nPorts)
    : sc_module(mn) , numPorts(nPorts) {

    switch( enc ) {
        case IMultiplexer::Binary:
            selSize = (unsigned short) log2(nPorts);
            u_SWITCH = new BinarySignalMux(mn,nPorts);
            break;
        case IMultiplexer::OneHot:
            selSize = nPorts;
            u_SWITCH = new OneHotSignalMux(mn,nPorts);
            break;
    }

    w_SEL.init(selSize);
    w_DATA_IN.init(nPorts);

    SC_CTHREAD(p_STIMULUS,i_CLK.pos());
    sensitive << i_CLK;

    tf = sc_create_vcd_trace_file("Multiplexer");
    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,w_DATA_OUT,"OUT");
    for( unsigned short i = 0; i < selSize; i++ ) {
        char buff[10];
        sprintf(buff,"SEL(%u)",i);
        sc_trace(tf,w_SEL[i],buff);
    }
    for( unsigned short i = 0; i < numPorts; i++ ) {
        char buff[10];
        sprintf(buff,"IN(%u)",i);
        sc_trace(tf,w_DATA_IN[i],buff);
    }

}

MultiplexerTestbench::~MultiplexerTestbench() {
    sc_close_vcd_trace_file(tf);
    delete u_SWITCH;
}


void MultiplexerTestbench::p_STIMULUS() {

    unsigned short i; // Loop iterator

    wait();

    for( i = 0; i < numPorts; i++ ) {
        w_DATA_IN[i].write( i+1 );
    }

    wait();

    // One-hot test
    for( i = 0; i < selSize; i++ ) {
        w_SEL[i].write(true);
        wait();
        w_SEL[i].write(false);
    }

    // Binary test
    for( i = 0; i < numPorts; i++ ) {
        for( unsigned short x = 0; x < selSize; x++ ) {
            unsigned short mask = (unsigned short) pow(2,x);
            unsigned index = i & mask;
            std::cout << "Valor: " << i << ", Bitpos: " << x << ", Mascara: " << mask << ", Index: " << index
                      << std::endl;
            if( index ) {
                w_SEL[x].write(true);
            } else{
                w_SEL[x].write(false);
            }
        }
        wait();
    }

    sc_stop();
}
