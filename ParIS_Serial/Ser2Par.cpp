#include "Ser2Par.h"

Ser2Par::Ser2Par(sc_module_name mn)
    : SoCINModule(mn),
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
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_NEXT_STATE);
    sensitive << r_CURRENT << i_VALID;

    SC_METHOD(p_MOUNT);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_OUTDATA);
    sensitive << r_DATA;

    SC_METHOD(p_OUTRETURN);
    sensitive << i_RETURN;

    SC_METHOD(p_OUTVALID);
    sensitive << r_CURRENT;
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
        case s_S0: {
            if( i_VALID.read() ) {
                r_NEXT.write(s_S1);
            } else {
                r_NEXT.write(s_S0);
            }
            break;
        }
        case s_S1:{
            if( i_VALID.read() == 0 ) {
                r_NEXT.write(s_S2);
            } else {
                r_NEXT.write(s_S1);
            }
            break;
        }
        case s_S2: {
            r_NEXT.write(s_S0);
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
    } else {
        if( i_CLK.read() ) {
            if( i_VALID.read() ) {
                Flit f = r_DATA.read();
                f.data = f.data.range( FLIT_WIDTH-2,0 ) & i_DATA;
                r_DATA.write( f );
            }
        }
    }
}

void Ser2Par::p_OUTDATA() {
    o_DATA.write( r_DATA.read() );
}

void Ser2Par::p_OUTRETURN() {
    o_RETURN.write( i_RETURN.read() );
}

void Ser2Par::p_OUTVALID() {
    o_VALID.write( (r_CURRENT.read() == s_S2 ? 1 : 0 ) );
}
