#include "Ser2Par.h"

//#define DEBUG_SER_TO_PAR

Ser2Par::Ser2Par(sc_module_name mn, unsigned short router, unsigned short portNumber)
    : SoCINModule(mn),
      router(router),
      portNumber(portNumber),
      i_CLK("Ser2Par_iCLK"),
      i_RST("Ser2Par_iRST"),
      i_VALID("Ser2Par_iVALID"),
      i_RETURN("Ser2Par_iRETURN"),
      i_DATA("Ser2Par_iDATA"),
      o_VALID("Ser2Par_oVALID"),
      o_RETURN("Ser2Par_oRETURN"),
      o_DATA("Ser2Par_oDATA")
{
    SC_METHOD(p_CURRENT_STATE);
    sensitive << i_CLK << i_RST;

    SC_METHOD(p_NEXT_STATE);
    sensitive << r_CURRENT << i_VALID << i_RETURN << r_BIT_POS;

    SC_METHOD(p_MOUNT);
    sensitive << i_CLK << i_RST;

    SC_METHOD(p_OUTDATA);
    sensitive << r_DATA;

    SC_METHOD(p_OUTRETURN);
    sensitive << i_RETURN;

    SC_METHOD(p_OUTVALID);
    sensitive << r_CURRENT;

#ifdef DEBUG_SER_TO_PAR
    char filename[30];
    sprintf(filename,"SerToPar(R%u_P%u)",router,portNumber);
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
    sc_trace(tf,r_DATA,"r_DATA");
    sc_trace(tf,r_BIT_POS,"r_POS");
#endif

}

Ser2Par::~Ser2Par() {
#ifdef DEBUG_SER_TO_PAR
    sc_close_vcd_trace_file(tf);
#endif
}

void Ser2Par::p_CURRENT_STATE() {
    if( i_RST.read() ) {
        r_CURRENT.write( s_S0 );
    } else {
        if( i_CLK.read() == 1 ) {
            r_CURRENT.write( r_NEXT.read() );
        }
    }
}

void Ser2Par::p_NEXT_STATE() {

    switch( r_CURRENT.read() ) {
        case s_S0:{
            if( i_VALID.read()==1 and i_RETURN.read() == 0 ) {
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
            if( i_RETURN.read() == 1 ) {
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

void Ser2Par::p_MOUNT() {
    static Flit fNull = 0;

    if( i_RST.read() ) {
        r_DATA.write( fNull );
        r_BIT_POS.write(0);
    } else {
        if( i_CLK.read() ) {
            if( i_VALID.read()==1 and (r_CURRENT.read() == s_S0 or r_CURRENT.read() == s_S1) ) {
                Flit f = r_DATA.read();
                f.packet_ptr = _packet_reference.read(); // Not circuit related

                f.data(FLIT_WIDTH-1,1) = f.data.range( FLIT_WIDTH-2,0 );
                f.data[0] = i_DATA.read();
                r_DATA.write( f );
                r_BIT_POS.write( r_BIT_POS.read() + 1 );

            } else {
                r_DATA.write( r_DATA.read() );
                r_BIT_POS.write(0);
            }
        }
    }
}

void Ser2Par::p_OUTDATA() {
    if( r_CURRENT.read() == s_S2 ) {
        o_DATA.write( r_DATA.read() );
    }
}

void Ser2Par::p_OUTRETURN() {
    o_RETURN.write( i_RETURN.read() );
}

void Ser2Par::p_OUTVALID() {
    if( r_CURRENT.read() == s_S2 ) {
        o_VALID.write(1);
//        printf("\n ---- >>> Ser2Par: Enviar !!!!!!!!!!!!");
    } else {
        o_VALID.write(0);
//        printf("\n ---- >>> Ser2Par: Nao enviar");
    }

//    o_VALID.write( (r_CURRENT.read() == s_S2 ? 1 : 0 ) );
}
