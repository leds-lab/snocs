#include "Demultiplexers.h"

/////////////////////////////////////////////////////////////
/// Testbench
/////////////////////////////////////////////////////////////
template<class DATA_TYPE>
DemultiplexerTestbench<DATA_TYPE>::DemultiplexerTestbench(sc_module_name mn,
                                                          IDemultiplexer<DATA_TYPE> *demux,
                                                          unsigned short nPorts)
    : sc_module(mn) , numPorts(nPorts) {

    if( BinaryDemux<DATA_TYPE>* dem = dynamic_cast<BinaryDemux<DATA_TYPE>*>(demux) ) {
        selSize = (unsigned short) log2(nPorts);
    } else {
        selSize = nPorts;
    }

    u_SWITCH = demux;

    // Vectors initialization
    w_SEL.init(selSize);
    w_DATA_OUT.init(nPorts);

    // Binding
    u_SWITCH->i_SEL(w_SEL);
    u_SWITCH->i_DATA(w_DATA_IN);
    u_SWITCH->o_DATA(w_DATA_OUT);

    // Testbench processes register
    SC_CTHREAD(p_STIMULUS,i_CLK.pos());
    sensitive << i_CLK;

    // Tracing
    tf = sc_create_vcd_trace_file("Demultiplexer");
    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,w_DATA_IN,"IN");
    for( unsigned short i = 0; i < selSize; i++ ) {
        char buff[10];
        sprintf(buff,"SEL(%u)",i);
        sc_trace(tf,w_SEL[i],buff);
    }
    for( unsigned short i = 0; i < numPorts; i++ ) {
        char buff[10];
        sprintf(buff,"OUT(%u)",i);
        sc_trace(tf,w_DATA_OUT[i],buff);
    }

}

template<class DATA_TYPE>
DemultiplexerTestbench<DATA_TYPE>::~DemultiplexerTestbench() {
    sc_close_vcd_trace_file(tf);
    delete u_SWITCH;
}

template<class DATA_TYPE>
void DemultiplexerTestbench<DATA_TYPE>::p_STIMULUS() {

    unsigned short i,x; // Loop iterator

    DATA_TYPE tmp = 0xF;

    wait();

    w_DATA_IN.write( tmp );
    wait();

    // One-hot test
    for( i = 0; i < selSize; i++ ) {
        w_SEL[i].write(true);
        wait();
        tmp = (i+1);
        w_DATA_IN.write( tmp );
        wait();
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
        tmp = (i+1)*(x+1);
        w_DATA_IN.write(tmp);
        wait();
    }

    sc_stop();
}

// Testbenchs tested
template class DemultiplexerTestbench<bool>;
template class DemultiplexerTestbench<sc_uint<34> >;
