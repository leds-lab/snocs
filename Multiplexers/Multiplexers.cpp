#include "Multiplexers.h"

/////////////////////////////////////////////////////////////
/// Testbench
/////////////////////////////////////////////////////////////
template<class DATA_TYPE>
MultiplexerTestbench<DATA_TYPE>::MultiplexerTestbench(sc_module_name mn,
                                           IMultiplexer<DATA_TYPE> *mux,
                                           unsigned short nPorts)
    : sc_module(mn) , numPorts(nPorts) {

    if( BinaryMux<DATA_TYPE>* m = dynamic_cast<BinaryMux<DATA_TYPE>*>(mux) ) {
        selSize = (unsigned short) log2(nPorts);
    } else {
        selSize = nPorts;
    }

    u_SWITCH = mux;

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

template<class DATA_TYPE>
MultiplexerTestbench<DATA_TYPE>::~MultiplexerTestbench() {
    sc_close_vcd_trace_file(tf);
    delete u_SWITCH;
}

template<class DATA_TYPE>
void MultiplexerTestbench<DATA_TYPE>::p_STIMULUS() {

    unsigned short i,x; // Loop iterator

    DATA_TYPE tmp;

    wait();

    for( i = 0; i < numPorts; i++ ) {
        tmp = i+1;
        w_DATA_IN[i].write( tmp );
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
            tmp = (i+1) * (x+1);
            w_DATA_IN[x].write(tmp);
        }

        w_SEL[i].write(false);
    }

    // Binary test
    for( i = 0; i < numPorts; i++ ) {
        for(  x = 0; x < selSize; x++ ) {
            unsigned short mask = (unsigned short) pow(2,x);
            unsigned index = i & mask;
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
            tmp = (i+1) * (x+1);
            w_DATA_IN[x].write(tmp);
        }
    }

    sc_stop();
}

// Testbenchs tested
template class MultiplexerTestbench<bool>;
template class MultiplexerTestbench<sc_uint<34> >;
