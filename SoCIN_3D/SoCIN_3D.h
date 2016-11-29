#ifndef __SOCIN_3D_H__
#define __SOCIN_3D_H__

#include "../NoC/NoC.h"

class SoCIN_3D : public INoC_VC {
public:
    // SIGNALS
    // Signals for the links on the X direction
    sc_vector<sc_signal<Flit> > w_X_DATA_TO_LEFT;
    sc_vector<sc_signal<bool> > w_X_VALID_TO_LEFT;
    sc_vector<sc_signal<bool> > w_X_RETURN_TO_LEFT;
    sc_vector<sc_vector<sc_signal<bool> > > w_X_VC_SELECTOR_TO_LEFT; // VC Link
    sc_vector<sc_signal<Flit> > w_X_DATA_TO_RIGHT;
    sc_vector<sc_signal<bool> > w_X_VALID_TO_RIGHT;
    sc_vector<sc_signal<bool> > w_X_RETURN_TO_RIGHT;
    sc_vector<sc_vector<sc_signal<bool> > > w_X_VC_SELECTOR_TO_RIGHT;// VC link

    // Signals for the links on the Y direction
    sc_vector<sc_signal<Flit> > w_Y_DATA_TO_SOUTH;
    sc_vector<sc_signal<bool> > w_Y_VALID_TO_SOUTH;
    sc_vector<sc_signal<bool> > w_Y_RETURN_TO_SOUTH;
    sc_vector<sc_vector<sc_signal<bool> > > w_Y_VC_SELECTOR_TO_SOUTH;// VC link
    sc_vector<sc_signal<Flit> > w_Y_DATA_TO_NORTH;
    sc_vector<sc_signal<bool> > w_Y_VALID_TO_NORTH;
    sc_vector<sc_signal<bool> > w_Y_RETURN_TO_NORTH;
    sc_vector<sc_vector<sc_signal<bool> > > w_Y_VC_SELECTOR_TO_NORTH;// VC link

    // Signals for the links on the Z direction
    sc_vector<sc_signal<Flit> > w_Z_DATA_TO_UP;
    sc_vector<sc_signal<bool> > w_Z_VALID_TO_UP;
    sc_vector<sc_signal<bool> > w_Z_RETURN_TO_UP;
    sc_vector<sc_vector<sc_signal<bool> > > w_Z_VC_SELECTOR_TO_UP;// VC link
    sc_vector<sc_signal<Flit> > w_Z_DATA_TO_DOWN;
    sc_vector<sc_signal<bool> > w_Z_VALID_TO_DOWN;
    sc_vector<sc_signal<bool> > w_Z_RETURN_TO_DOWN;
    sc_vector<sc_vector<sc_signal<bool> > > w_Z_VC_SELECTOR_TO_DOWN;// VC link

    sc_trace_file* tf;

    SC_HAS_PROCESS(SoCIN_3D);
    SoCIN_3D(sc_module_name mn);

    const char* moduleName() const { return "SoCIN-3D"; }
    bool isTopology3D() const { return true; }

    ~SoCIN_3D();
};

#endif // SOCIN_3D_H
