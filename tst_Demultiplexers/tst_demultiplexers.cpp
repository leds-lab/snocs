#include "../src/Demultiplexers.h"

using namespace std;
using namespace sc_dt;

/////////////////////////////////////////////////////////////
/// Demultiplexer Testbench
/////////////////////////////////////////////////////////////
template<class DATA_TYPE>
class DemultiplexerTestbench : public sc_module {
protected:
    unsigned short numPorts;
    unsigned short selSize;
public:
    sc_in<bool> i_CLK; // Clock

    sc_vector<sc_signal<bool> >      w_SEL;      // Selector
    sc_signal<DATA_TYPE>             w_DATA_IN;  // Input
    sc_vector<sc_signal<DATA_TYPE> > w_DATA_OUT; // Outputs

    // DUT - Design Under Test
    IDemultiplexer<DATA_TYPE>* u_SWITCH;

    // Trace file
    sc_trace_file* tf;

    void p_STIMULUS();

    SC_HAS_PROCESS(DemultiplexerTestbench);
    DemultiplexerTestbench(sc_module_name mn, IDemultiplexer<DATA_TYPE>* demux, unsigned short nPorts);

    ~DemultiplexerTestbench();
};

template<class DATA_TYPE>
inline DemultiplexerTestbench<DATA_TYPE>::DemultiplexerTestbench(sc_module_name mn,
                                                          IDemultiplexer<DATA_TYPE> *demux,
                                                          unsigned short nPorts)
    : sc_module(mn) , numPorts(nPorts) {

    if( BinaryDemux<DATA_TYPE>* dem = dynamic_cast<BinaryDemux<DATA_TYPE>*>(demux) ) {
        selSize = (unsigned short) ceil(log2(nPorts));
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
inline DemultiplexerTestbench<DATA_TYPE>::~DemultiplexerTestbench() {
    sc_close_vcd_trace_file(tf);
    delete u_SWITCH;
}

template<class DATA_TYPE>
inline void DemultiplexerTestbench<DATA_TYPE>::p_STIMULUS() {

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
            unsigned short mask = (unsigned short) pow(2.f,x);
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


int sc_main(int argc, char *argv[]) {

    if( argc < 3) {
        std::cout << "Missing arguments...\n ./tst_demultiplexers <bin|one-hot> <numPorts>" << std::endl;
        return -1;
    }

    char* type = argv[1];
    unsigned short nPorts = (unsigned short) atoi(argv[2]);

    IDemultiplexer<sc_uint<34> >* u_DEMUX = NULL;

    if( strcmp(type,"bin") == 0 ) {
        u_DEMUX = new BinaryDemux<sc_uint<34> >("Bin_Demux",nPorts);
        std::cout << "\nBinary demux with " << nPorts << " ports" << std::endl;
    } else {
        u_DEMUX = new OneHotDemux<sc_uint<34> >("OneHot_Demux",nPorts);
        std::cout << "\nOne-hot demux with " << nPorts << " ports" << std::endl;
    }

    DemultiplexerTestbench<sc_uint<34> >* u_TB =
            new DemultiplexerTestbench<sc_uint<34> >("Testbench",u_DEMUX,nPorts);

    sc_clock w_CLK("CLK",10,SC_NS);
    u_TB->i_CLK(w_CLK);

    sc_start();

    delete u_TB;

    return 0;
}
