#include "Par2Ser.h"

Par2Ser::Par2Ser(sc_module_name mn)
    : SoCINModule(mn),
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
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_NEXT_STATE);
    sensitive << r_CURRENT << i_VALID << r_BIT_POS;

    SC_METHOD(p_COUNT);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_OUTDATA);
    sensitive << r_DATA;

    SC_METHOD(p_OUTRETURN);
    sensitive << i_RETURN;

    SC_METHOD(p_OUTVALID);
    sensitive << r_CURRENT;
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
        case s_S0: {
            if( i_VALID.read() ) {
                r_NEXT.write(s_S1);
            } else {
                r_NEXT.write(s_S0);
            }
            break;
        }
        case s_S1:{
            if( r_BIT_POS.read() == FLIT_WIDTH-1 ) {
                r_NEXT.write(s_S0);
            } else {
                r_NEXT.write(s_S1);
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
            if( i_VALID.read() ) {
                r_DATA.write( i_DATA.read() );
                r_BIT_POS.write(0);
            } else {
                Flit f = r_DATA.read();
                f.data = f.data.range( FLIT_WIDTH-2,0 ) & 0;
                r_DATA.write( f );
                r_BIT_POS.write( r_BIT_POS.read() + 1 );
            }
        }
    }
}

void Par2Ser::p_OUTDATA() {
    Flit f = r_DATA.read();
    o_DATA.write( f.data.bit(FLIT_WIDTH-1) );
}

void Par2Ser::p_OUTVALID() {
    o_VALID.write( (r_CURRENT.read() == s_S1 ? 1 : 0 ) );
}

void Par2Ser::p_OUTRETURN() {
    o_RETURN.write( i_RETURN.read() );
}
