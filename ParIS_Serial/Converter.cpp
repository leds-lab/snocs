#include "Converter.h"

Converter::Converter(sc_module_name mn, unsigned short router, unsigned short portNumber)
    : SoCINModule(mn),
      router(router),
      portNumber(portNumber)
{
    // Instantiating and binding Parallel to Serial unit
    u_PAR_SER = new Par2Ser("Parallel_To_Serial",router,portNumber);
    u_PAR_SER->i_CLK(i_CLK);
    u_PAR_SER->i_RST(i_RST);
    u_PAR_SER->i_VALID(i_VALID);
    u_PAR_SER->i_RETURN(w_RETURN);
    u_PAR_SER->i_DATA(i_DATA);
    u_PAR_SER->o_VALID(w_VALID);
    u_PAR_SER->o_RETURN(o_RETURN);
    u_PAR_SER->o_DATA(w_DATA);

    u_PAR_SER->_packet_reference(_packet_reference);

    // Instantiating and binding Serial to Parallel unit
    u_SER_PAR = new Ser2Par("Serial_To_Parallel",router,portNumber);
    u_SER_PAR->i_CLK(i_CLK);
    u_SER_PAR->i_RST(i_RST);
    u_SER_PAR->i_VALID(w_VALID);
    u_SER_PAR->i_RETURN(i_RETURN);
    u_SER_PAR->i_DATA(w_DATA);
    u_SER_PAR->o_VALID(o_VALID);
    u_SER_PAR->o_RETURN(w_RETURN);
    u_SER_PAR->o_DATA(o_DATA);

    u_SER_PAR->_packet_reference(_packet_reference);
}

Converter::~Converter() {
    delete u_PAR_SER;
    delete u_SER_PAR;
}
