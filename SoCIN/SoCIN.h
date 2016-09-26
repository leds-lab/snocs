#ifndef SOCIN_H
#define SOCIN_H

#include "../NoC/NoC.h"

class SoCIN : public INoC {
public:

    // SIGNALS
    // Sinals for the links on the X direction
    sc_vector<sc_signal<Flit> > w_X_DATA_IN;
    sc_vector<sc_signal<bool> > w_X_VALID_IN;
    sc_vector<sc_signal<bool> > w_X_RETURN_IN;
    sc_vector<sc_signal<Flit> > w_X_DATA_OUT;
    sc_vector<sc_signal<bool> > w_X_VALID_OUT;
    sc_vector<sc_signal<bool> > w_X_RETURN_OUT;

    // Sinals for the links on the Y direction
    sc_vector<sc_signal<Flit> > w_Y_DATA_IN;
    sc_vector<sc_signal<bool> > w_Y_VALID_IN;
    sc_vector<sc_signal<bool> > w_Y_RETURN_IN;
    sc_vector<sc_signal<Flit> > w_Y_DATA_OUT;
    sc_vector<sc_signal<bool> > w_Y_VALID_OUT;
    sc_vector<sc_signal<bool> > w_Y_RETURN_OUT;

    SC_HAS_PROCESS(INoC);
    SoCIN(sc_module_name mn);

    const char* moduleName() const { return "SoCIN"; }

    ~SoCIN();
};

#endif // SOCIN_H
