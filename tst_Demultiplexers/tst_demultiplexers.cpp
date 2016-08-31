#include "../Demultiplexers/Demultiplexers.h"

using namespace std;

int sc_main(int argc, char *argv[]) {

    if( argc < 3) {
        std::cout << "Missing arguments...\n ./tst_demultiplexers <bin|one-hot> <numPorts>" << std::endl;
        return -1;
    }

    char* type = argv[1];
    unsigned short nPorts = (unsigned short) atoi(argv[2]);

    IDemultiplexer<sc_uint<DATA_WIDTH> >* u_DEMUX = NULL;

    if( strcmp(type,"bin") == 0 ) {
        u_DEMUX = new BinaryDemux<sc_uint<DATA_WIDTH> >("Bin_Demux",nPorts);
        std::cout << "\nBinary demux with " << nPorts << " ports" << std::endl;
    } else {
        u_DEMUX = new OneHotDemux<sc_uint<DATA_WIDTH> >("OneHot_Demux",nPorts);
        std::cout << "\nOne-hot demux with " << nPorts << " ports" << std::endl;
    }

    DemultiplexerTestbench<sc_uint<DATA_WIDTH> >* u_TB =
            new DemultiplexerTestbench<sc_uint<DATA_WIDTH> >("Testbench",u_DEMUX,nPorts);

    sc_clock w_CLK("CLK",10,SC_NS);
    u_TB->i_CLK(w_CLK);

    sc_start();

    delete u_TB;

    return 0;
}
