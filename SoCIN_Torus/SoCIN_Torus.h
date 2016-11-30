#ifndef __SOCIN_TORUS_H__
#define __SOCIN_TORUS_H__

#include "../NoC/NoC.h"

class SoCIN_Torus : public INoC_VC {
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

    // Waveform for debug proposal
    sc_trace_file* tf;

    SC_HAS_PROCESS(SoCIN_Torus);
    SoCIN_Torus(sc_module_name mn);

    const char* moduleName() const { return "SoCIN_Torus"; }
    TopologyType topologyType() const { return INoC::TT_Orthogonal2D; }

    ~SoCIN_Torus();
};

#endif // __SOCIN_TORUS_H__
