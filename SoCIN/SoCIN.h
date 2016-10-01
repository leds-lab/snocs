#ifndef SOCIN_H
#define SOCIN_H

#include "../NoC/NoC.h"

class SoCIN : public INoC {
public:

    // SIGNALS
    // Sinals for the links on the X direction
    sc_vector<sc_signal<Flit> > w_X_DATA_TO_LEFT;
    sc_vector<sc_signal<bool> > w_X_VALID_TO_LEFT;
    sc_vector<sc_signal<bool> > w_X_RETURN_TO_LEFT;
    sc_vector<sc_signal<Flit> > w_X_DATA_TO_RIGHT;
    sc_vector<sc_signal<bool> > w_X_VALID_TO_RIGHT;
    sc_vector<sc_signal<bool> > w_X_RETURN_TO_RIGHT;

    // Sinals for the links on the Y direction
    sc_vector<sc_signal<Flit> > w_Y_DATA_TO_SOUTH;
    sc_vector<sc_signal<bool> > w_Y_VALID_TO_SOUTH;
    sc_vector<sc_signal<bool> > w_Y_RETURN_TO_SOUTH;
    sc_vector<sc_signal<Flit> > w_Y_DATA_TO_NORTH;
    sc_vector<sc_signal<bool> > w_Y_VALID_TO_NORTH;
    sc_vector<sc_signal<bool> > w_Y_RETURN_TO_NORTH;

    void p_DEBUG();

    sc_trace_file* tf;

    SC_HAS_PROCESS(SoCIN);
    SoCIN(sc_module_name mn);

    const char* moduleName() const { return "SoCIN"; }

    ~SoCIN();
};

#endif // SOCIN_H
