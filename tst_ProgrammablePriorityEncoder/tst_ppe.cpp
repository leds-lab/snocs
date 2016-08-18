#include "../ProgrammablePriorityEncoder/ProgrammablePriorityEncoder.h"

int sc_main(int , char **) {

    sc_clock w_CLK("CLK",10,SC_NS);

    tst_PPE* tb = new tst_PPE("PPE_Testbench",4);
    tb->i_CLK(w_CLK);

    sc_start();

    delete tb;

    return 0;
}
