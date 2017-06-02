#include "../src/And.h"

using namespace sc_core;
using namespace sc_dt;

class tst_And : public sc_module {
private:
    unsigned short numPorts;
public:
    sc_in<bool>                 i_CLK;
    sc_vector<sc_signal<bool> > w_DATA_IN;
    sc_signal<bool>             w_DATA_OUT;

    void p_STIMULUS();

    And* u_AND;

    sc_trace_file *tf;

    SC_HAS_PROCESS(tst_And);
    tst_And(sc_module_name mn, unsigned short nPorts)
        : sc_module(mn), numPorts(nPorts),
          i_CLK("tst_And_iCLK"),
          w_DATA_IN("tst_And_wDATA_IN",nPorts),
          w_DATA_OUT("tst_And_wDATA_OUT")
    {
        u_AND = new And("ANDPARAM",nPorts);
        u_AND->i_DATA(w_DATA_IN);
        u_AND->o_DATA(w_DATA_OUT);

        tf = sc_create_vcd_trace_file("AndParam");
        sc_trace(tf,i_CLK,"CLK");
        sc_trace(tf,w_DATA_OUT,"OUT");
        for(unsigned short i = 0; i < nPorts; i++) {
            char strIn[10];
            sprintf(strIn,"IN(%u)",i);
            sc_trace(tf,w_DATA_IN[i],strIn);
        }

        SC_CTHREAD(p_STIMULUS,i_CLK.pos());
        sensitive << i_CLK;
    }
    ~tst_And() {
        sc_close_vcd_trace_file(tf);
        delete u_AND;
    }
};

inline void tst_And::p_STIMULUS() {

    unsigned short i;

    sc_uint<16> v_DATA;
    for( i = 0; i < numPorts*numPorts; i++ ) {
        v_DATA = i;
        for( unsigned int x = 0; x < numPorts; x++ ) {
            w_DATA_IN[x].write( v_DATA[x] );
        }
        wait();
    }

    sc_stop();
}

int sc_main(int , char **) {

    sc_clock w_CLK("CLK",10,SC_NS);

    tst_And* tb = new tst_And("TB_AND",4);
    tb->i_CLK(w_CLK);

    sc_start();

    delete tb;

    return 0;
}
