#include "../Multiplexers/Multiplexers.h"

using namespace std;
using namespace sc_dt;

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

/////////////////////////////////////////////////////////////
/// Testbench
/////////////////////////////////////////////////////////////
template<class DATA_TYPE>
inline MultiplexerTestbench<DATA_TYPE>::MultiplexerTestbench(sc_module_name mn,
                                           IMultiplexer<DATA_TYPE> *mux,
                                           unsigned short nPorts)
    : sc_module(mn) , numPorts(nPorts) {

    if( BinaryMux<DATA_TYPE>* m = dynamic_cast<BinaryMux<DATA_TYPE>*>(mux) ) {
        selSize = (unsigned short) ceil(log2(nPorts));
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
inline MultiplexerTestbench<DATA_TYPE>::~MultiplexerTestbench() {
    sc_close_vcd_trace_file(tf);
    delete u_SWITCH;
}

template<class DATA_TYPE>
inline void MultiplexerTestbench<DATA_TYPE>::p_STIMULUS() {

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
            unsigned short mask = (unsigned short) pow(2.f,x);
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


int sc_main(int argc, char *argv[]) {

    if( argc < 3) {
        std::cout << "Missing arguments...\n ./tst_multiplexers <bin|one-hot> <numPorts>" << std::endl;
        return -1;
    }

    char* type = argv[1];
    unsigned short nPorts = (unsigned short) atoi(argv[2]);

    IMultiplexer<sc_uint<34> >* u_MUX = NULL;

    if( strcmp(type,"bin") == 0 ) {
        u_MUX = new BinaryMux<sc_uint<34> >("Bin_Mux",nPorts);
        std::cout << "\nBinary mux with " << nPorts << " ports" << std::endl;
    } else {
        u_MUX = new OneHotMux<sc_uint<34> >("OneHot_Mux",nPorts);
        std::cout << "\nOne-hot mux with " << nPorts << " ports" << std::endl;
    }

    MultiplexerTestbench<sc_uint<34> >* u_TB = new MultiplexerTestbench<sc_uint<34> >("Testbench",u_MUX,nPorts);

    sc_clock w_CLK("CLK",10,SC_NS);
    u_TB->i_CLK(w_CLK);

    sc_start();

    delete u_TB;

    return 0;
}
