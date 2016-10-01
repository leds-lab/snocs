#include "SystemSignals.h"

using namespace sc_core;
using namespace sc_dt;

SystemSignals::SystemSignals(sc_module_name mn)
    : sc_module(mn),
      i_CLK("Sys_iCLK"),
      o_RST("Sys_oRST"),
      o_GLOBAL_CLOCK("Sys_oGLOBAL_CLOCK")
{

    r_COUNTER = 0;

    SC_CTHREAD(p_CLOCK,i_CLK.pos());
    sensitive << i_CLK.pos() << o_RST.pos();

    SC_CTHREAD(p_RESET,i_CLK.pos());
    sensitive << i_CLK.pos();

}

void SystemSignals::p_CLOCK() {
    if (o_RST.read()==1) {
        r_COUNTER = 0;
        o_GLOBAL_CLOCK.write(0);
        wait();
    } else {
        while(1) {
            r_COUNTER = r_COUNTER + 1;
            o_GLOBAL_CLOCK.write(r_COUNTER);
            wait();
        }
    }
}

void SystemSignals::p_RESET() {

    o_RST.write(1);
    wait();

    o_RST.write(0);
    wait();
    while(1) wait();
}
