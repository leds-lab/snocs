#ifndef __SOCIN_RING_H__
#define __SOCIN_RING_H__

#include "../NoC/NoC.h"

class SoCIN_Ring : public INoC_VC {
public:

    // SIGNALS
    // Signals for the links on the X direction
    sc_vector<sc_signal<Flit> > w_DATA_TO_LEFT;
    sc_vector<sc_signal<bool> > w_VALID_TO_LEFT;
    sc_vector<sc_signal<bool> > w_RETURN_TO_LEFT;
    sc_vector<sc_vector<sc_signal<bool> > > w_VC_SELECTOR_TO_LEFT; // VC Link
    sc_vector<sc_signal<Flit> > w_DATA_TO_RIGHT;
    sc_vector<sc_signal<bool> > w_VALID_TO_RIGHT;
    sc_vector<sc_signal<bool> > w_RETURN_TO_RIGHT;
    sc_vector<sc_vector<sc_signal<bool> > > w_VC_SELECTOR_TO_RIGHT;// VC link

    // Trace file for debug proposal (waveform)
    sc_trace_file* tf;

    SC_HAS_PROCESS(SoCIN_Ring);
    SoCIN_Ring(sc_module_name mn);

    const char* moduleName() const { return "SoCIN_Ring"; }
    TopologyType topologyType() const { return INoC::TT_Non_Orthogonal; }

    ~SoCIN_Ring();
};

#endif // __SOCIN_RING_H__
