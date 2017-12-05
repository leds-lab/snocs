#ifndef __CONVERTER_H__
#define __CONVERTER_H__

#include "Par2Ser.h"
#include "Ser2Par.h"

class Converter : public SoCINModule {
public:

    // Interface
    sc_in<bool>  i_CLK;
    sc_in<bool>  i_RST;
    sc_in<bool>  i_VALID;
    sc_in<bool>  i_RETURN;
    sc_in<Flit>  i_DATA;
    sc_out<bool> o_VALID;
    sc_out<bool> o_RETURN;
    sc_out<Flit> o_DATA;

    // Wires
    sc_signal<bool> w_VALID;
    sc_signal<bool> w_RETURN;
    sc_signal<bool> w_DATA;

    // Internal units
    Par2Ser* u_PAR_SER;
    Ser2Par* u_SER_PAR;

    Converter(sc_module_name mn);

    const char* moduleName() const { return "Converter_Parallel_Serial_Parallel"; }
    ModuleType moduleType() const { return SoCINModule::OtherT; }

    ~Converter();
};

#endif // CONVERTER_H
