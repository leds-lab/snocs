#include "../Multiplexers/Multiplexers.h"

using namespace std;

int sc_main(int argc, char *argv[]) {

    if( argc < 3) {
        std::cout << "Missing arguments...\n ./tst_multiplexers <bin|one-hot> <numPorts>" << std::endl;
        return -1;
    }

    char* type = argv[1];
    unsigned short nPorts = (unsigned short) atoi(argv[2]);

    IMultiplexer<sc_uint<DATA_WIDTH> >* u_MUX = NULL;

    if( strcmp(type,"bin") == 0 ) {
        u_MUX = new BinaryMux<sc_uint<DATA_WIDTH> >("Bin_Mux",nPorts);
        std::cout << "\nBinary mux with " << nPorts << " ports" << std::endl;
    } else {
        u_MUX = new OneHotMux<sc_uint<DATA_WIDTH> >("OneHot_Mux",nPorts);
        std::cout << "\nOne-hot mux with " << nPorts << " ports" << std::endl;
    }

    MultiplexerTestbench<sc_uint<DATA_WIDTH> >* u_TB = new MultiplexerTestbench<sc_uint<DATA_WIDTH> >("Testbench",u_MUX,nPorts);

    sc_clock w_CLK("CLK",10,SC_NS);
    u_TB->i_CLK(w_CLK);

    sc_start();

    delete u_TB;

    return 0;
}
