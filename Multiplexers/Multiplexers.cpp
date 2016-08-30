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
        o_DATA.write( i_DATA[i].read() );
    } else {
        o_DATA.write( 0 );
    }

}


////////////////////////////////////////////////////////////////////////////////////////////


BinarySignalMux::BinarySignalMux(sc_module_name mn, unsigned short nPorts)
    : IMultiplexer(mn,nPorts)
{
    selSize = (unsigned short) log2(nPorts);

    i_SEL.init( selSize );

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

    for( i = selSize-1; i != 0; i-- ) {
        sel = (sel << 1) | i_SEL[i].read();
    }
    sel = (sel << 1) | i_SEL[0].read();

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

    // Unit instantiation
    switch( enc ) {
        case IMultiplexer::Binary:
            selSize = (unsigned short) log2(nPorts);
            u_SWITCH = new BinarySignalMux("BinMux",nPorts);
            break;
        case IMultiplexer::OneHot:
            selSize = nPorts;
            u_SWITCH = new OneHotSignalMux("One-HotMux",nPorts);
            break;
    }

    // Vectors initialization
    w_SEL.init(selSize);
    w_DATA_IN.init(nPorts);

    // Binding
    u_SWITCH->i_SEL(w_SEL);
    u_SWITCH->i_DATA(w_DATA_IN);
    u_SWITCH->o_DATA(w_DATA_OUT);

    // Testbench processes register
    SC_CTHREAD(p_STIMULUS,i_CLK.pos());
    sensitive << i_CLK;

    // Tracing
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

    unsigned short i,x; // Loop iterator

    wait();

    for( i = 0; i < numPorts; i++ ) {
        w_DATA_IN[i].write( true );
    }
    wait();

    // One-hot test
    for( i = 0; i < selSize; i++ ) {
        w_SEL[i].write(true);
        wait();
        for( x = numPorts-1; x != 0; x--) {
            w_DATA_IN[x].write(false);
            wait();
        }
        w_DATA_IN[0].write(false);
        wait();
        for( x = 0; x < numPorts; x++) {
            w_DATA_IN[x].write(true);
        }

        w_SEL[i].write(false);
    }

    // Binary test
    for( i = 0; i < numPorts; i++ ) {
        for(  x = 0; x < selSize; x++ ) {
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

        for( x = numPorts-1; x != 0; x--) {
            w_DATA_IN[x].write(false);
            wait();
        }
        w_DATA_IN[0].write(false);
        wait();
        for( x = 0; x < numPorts; x++) {
            w_DATA_IN[x].write(true);
        }
    }

    sc_stop();
}
