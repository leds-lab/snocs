#include "Par2Ser.h"

#include <systemc>

//#define DEBUG_PAR_TO_SER

Par2Ser::Par2Ser(sc_module_name mn, unsigned short router, unsigned short portNumber)
    : SoCINModule(mn),
      router(router),
      portNumber(portNumber),
      i_CLK("Par2Ser_iCLK"),
      i_RST("Par2Ser_iRST"),
      i_VALID("Par2Ser_iVALID"),
      i_RETURN("Par2Ser_iRETURN"),
      i_DATA("Par2Ser_iDATA"),
      o_VALID("Par2Ser_oVALID"),
      o_RETURN("Par2Ser_oRETURN"),
      o_DATA("Par2Ser_oDATA")
{
    SC_METHOD(p_CURRENT_STATE);
    sensitive << i_CLK << i_RST;

    SC_METHOD(p_NEXT_STATE);
    sensitive << r_CURRENT << i_VALID << r_BIT_POS << i_RETURN;

    SC_METHOD(p_COUNT);
    sensitive << i_CLK << i_RST;

    SC_METHOD(p_OUTDATA);
    sensitive << r_DATA;

    SC_METHOD(p_OUTRETURN);
    sensitive << i_RETURN;

    SC_METHOD(p_OUTVALID);
    sensitive << r_CURRENT;

#ifdef DEBUG_PAR_TO_SER
    char filename[30];
    sprintf(filename,"ParToSer(R%u_P%u)",router,portNumber);
    tf = sc_create_vcd_trace_file(filename);
    sc_trace(tf,i_CLK,"CLK");
    sc_trace(tf,i_RST,"RST");
    sc_trace(tf,i_VALID,"i_VAL");
    sc_trace(tf,i_RETURN,"i_RET");
    sc_trace(tf,i_DATA,"i_DIN");
    sc_trace(tf,o_VALID,"o_VAL");
    sc_trace(tf,o_RETURN,"o_RET");
    sc_trace(tf,o_DATA,"o_DOUT");

    sc_trace(tf,r_CURRENT,"r_STATE");
    sc_trace(tf,r_NEXT,"r_NEXT_STATE");
    sc_trace(tf,r_BIT_POS,"r_POS");
    sc_trace(tf,r_DATA,"r_DATA");
#endif

}

Par2Ser::~Par2Ser() {
#ifdef DEBUG_PAR_TO_SER
    sc_close_vcd_trace_file(tf);
#endif
}

void Par2Ser::p_CURRENT_STATE() {
    if( i_RST.read() ) {
        r_CURRENT.write( s_S0 );
    } else {
        if( i_CLK.read() == 1 ) {
            r_CURRENT.write( r_NEXT.read() );
        }
    }
}

void Par2Ser::p_NEXT_STATE() {

    switch( r_CURRENT.read() ) {
        case s_S0:{
            if( i_VALID.read()==1 and i_RETURN.read()==0 ) {
                r_NEXT.write(s_S1);
            } else {
                r_NEXT.write(s_S0);
            }
            break;
        }
        case s_S1:{
            if( r_BIT_POS.read() == FLIT_WIDTH-1 ) {
                r_NEXT.write(s_S2);
            } else {
                r_NEXT.write(s_S1);
            }
            break;
        }
        case s_S2:{
            if( i_VALID.read() == 0 and i_RETURN.read()==1 ) {
                r_NEXT.write(s_S0);
            } else {
                r_NEXT.write(s_S2);
            }
            break;
        }
        default:{
            r_NEXT.write(s_S0);
        }
    }
}

void Par2Ser::p_COUNT() {
    static Flit fNull = 0;

    if( i_RST.read() ) {
        r_DATA.write( fNull );
        r_BIT_POS.write(0);
    } else {
        if( i_CLK.read() ) {
            if( r_CURRENT.read() == s_S1 ) {
                Flit f = r_DATA.read();
                _packet_reference.write( i_DATA.read().packet_ptr ); // Not circuit related

                f.data(FLIT_WIDTH-1,1) = f.data.range( FLIT_WIDTH-2,0 );
                f.data[0] = 0;
                r_DATA.write( f );
                r_BIT_POS.write( r_BIT_POS.read() + 1 );
            } else {
                r_DATA.write( i_DATA.read() );
                r_BIT_POS.write(0);
            }
        }
    }
}

void Par2Ser::p_OUTDATA() {
    Flit f = r_DATA.read();
    o_DATA.write( f.data[FLIT_WIDTH-1] );
}

void Par2Ser::p_OUTVALID() {
    if( r_CURRENT.read() == s_S1 or r_CURRENT.read() == s_S2 ) {
        o_VALID.write(1);
    } else {
        o_VALID.write(0);
    }
}

void Par2Ser::p_OUTRETURN() {
    o_RETURN.write( i_RETURN.read() );
}
