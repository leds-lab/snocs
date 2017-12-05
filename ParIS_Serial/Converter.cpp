#include "Converter.h"

Converter::Converter(sc_module_name mn)
    : SoCINModule(mn)
{
    // Instantiating and binding Parallel to Serial unit
    u_PAR_SER = new Par2Ser("Parallel_To_Serial");
    u_PAR_SER->i_CLK(i_CLK);
    u_PAR_SER->i_RST(i_RST);
    u_PAR_SER->i_VALID(i_VALID);
    u_PAR_SER->i_RETURN(w_RETURN);
    u_PAR_SER->i_DATA(i_DATA);
    u_PAR_SER->o_VALID(w_VALID);
    u_PAR_SER->o_RETURN(o_RETURN);
    u_PAR_SER->o_DATA(w_DATA);

    // Instantiating and binding Serial to Parallel unit
    u_SER_PAR = new Ser2Par("Serial_To_Parallel");
    u_SER_PAR->i_CLK(i_CLK);
    u_SER_PAR->i_RST(i_RST);
    u_SER_PAR->i_VALID(w_VALID);
    u_SER_PAR->i_RETURN(i_RETURN);
    u_SER_PAR->i_DATA(w_DATA);
    u_SER_PAR->o_VALID(o_VALID);
    u_SER_PAR->o_RETURN(w_RETURN);
    u_SER_PAR->o_DATA(o_DATA);
}

Converter::~Converter() {
    delete u_PAR_SER;
    delete u_SER_PAR;
}
