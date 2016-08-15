#include "Arbiter.h"

Arbiter::Arbiter(sc_module_name mn, IPriorityGenerator *pg, ProgrammablePriorityEncoder *ppe,
                 unsigned short nPorts, unsigned short XID, unsigned short YID, unsigned short PORT_ID)
        : sc_module(mn), nPorts(nPorts), i_CLK("Arb_iCLK"), i_RST("Arb_iRST"), i_REQUEST("Arb_REQ",nPorts),
          o_GRANT("Arb_oGRANT",nPorts),o_IDLE("Arb_oIDLE"), w_PRIORITY("Arb_wPRIOR",nPorts), XID(XID),YID(YID),
          PORT_ID(PORT_ID), u_PG(pg), u_PPE(ppe)
{

    // Binding ports
    // PPE
    u_PPE->i_CLK(i_CLK);
    u_PPE->i_RST(i_RST);
    u_PPE->i_REQUEST(i_REQUEST);
    u_PPE->i_PRIORITY(w_PRIORITY);
    u_PPE->o_GRANT(o_GRANT);
    u_PPE->o_IDLE(o_IDLE);

    // PG
    u_PG->i_CLK(i_CLK);
    u_PG->i_RST(i_RST);
    u_PG->i_GRANTS(o_GRANT);
    u_PG->o_PRIORITIES(w_PRIORITY);

}


/////////// Testbench ///////////
tst_Arbiter::tst_Arbiter(sc_module_name mn, Arbiter *arb, unsigned short nPorts)
        : sc_module(mn), nPorts(nPorts), i_CLK("tst_Arb_iCLK"), w_RST("tst_Arb_wRST"),
          w_REQUEST("tst_Arb_wREQ",nPorts), w_GRANT("tst_Arb_wGRANT",nPorts),
          w_IDLE("tst_Arb_wIDLE"),arb(arb) {

    // Bind
    arb->i_CLK(i_CLK);
    arb->i_RST(w_RST);
    arb->i_REQUEST(w_REQUEST);
    arb->o_GRANT(w_GRANT);
    arb->o_IDLE(w_IDLE);

    // Defining testbench stimulus process
    SC_CTHREAD(p_stimulus,i_CLK.pos());
    sensitive << i_CLK;

    // Trace
    tf = sc_create_vcd_trace_file("arbiter_waves");
    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,w_RST,"RST");
    sc_trace(tf,w_IDLE,"IDLE");
    for(unsigned short i = 0; i < nPorts; i++) {
        char strReq[10];
        sprintf(strReq,"REQ(%u)",i);
        sc_trace(tf,w_REQUEST[i],strReq);

        char strGnt[15];
        sprintf(strGnt,"GRANT(%u)",i);
        sc_trace(tf,w_GRANT[i],strGnt);
    }

}

tst_Arbiter::~tst_Arbiter() {
    sc_close_vcd_trace_file(tf);
    delete arb;
}

void tst_Arbiter::p_stimulus() {

    unsigned short i;

    // Initialize reseted
    w_RST.write(true);
    wait();

    w_RST.write(false);
    wait();

    for( unsigned short x = 0; x < nPorts; x++ ) {
        // All request OFF
        for(i = 0; i < nPorts; i++) {
            w_REQUEST[i].write(false);
        }
        wait();

        // All request ON
        for(i = 0; i < nPorts; i++) {
            w_REQUEST[i].write(true);
        }
        wait();
    }

    // Disabling one request per cycle
    for( i = 0; i < nPorts; i++ ) {
        w_REQUEST[i].write(false);
        wait();
    }

    // Specific for 4 bit
    if( nPorts == 4 ) {
        // 0b0001
        w_REQUEST[0].write(true);
        wait();

        // 0b0000
        w_REQUEST[0].write(false);
        wait();

        // 0b1001
        w_REQUEST[0].write(true);
        w_REQUEST[3].write(true);
        wait();

    }

    for( unsigned short x = 0; x < 1; x++ ) {
        // All request OFF
        for(i = 0; i < nPorts; i++) {
            w_REQUEST[i].write(false);
        }
        wait();

        // All request ON
        for(i = 0; i < nPorts; i++) {
            w_REQUEST[i].write(true);
        }
        wait();
    }

    // All request OFF
    for(i = 0; i < nPorts; i++) {
        w_REQUEST[i].write(false);
    }
    wait();


    sc_stop();
}
