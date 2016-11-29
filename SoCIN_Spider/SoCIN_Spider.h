#ifndef __SOCIN_SPIDER_H__
#define __SOCIN_SPIDER_H__

#include "../NoC/NoC.h"

class SoCIN_Spider : public INoC_VC {
public:

    // SIGNALS
    // Signals for the links on the LEFT direction
    sc_vector<sc_signal<Flit> > w_DATA_TO_LEFT;
    sc_vector<sc_signal<bool> > w_VALID_TO_LEFT;
    sc_vector<sc_signal<bool> > w_RETURN_TO_LEFT;
    sc_vector<sc_vector<sc_signal<bool> > > w_VC_SELECTOR_TO_LEFT; // VC Link
    // Signals for the links on the RIGHT direction
    sc_vector<sc_signal<Flit> > w_DATA_TO_RIGHT;
    sc_vector<sc_signal<bool> > w_VALID_TO_RIGHT;
    sc_vector<sc_signal<bool> > w_RETURN_TO_RIGHT;
    sc_vector<sc_vector<sc_signal<bool> > > w_VC_SELECTOR_TO_RIGHT;// VC link
    // Signals for the links on the ACROSS direction
    sc_vector<sc_signal<Flit> > w_DATA_ACROSS;
    sc_vector<sc_signal<bool> > w_VALID_ACROSS;
    sc_vector<sc_signal<bool> > w_RETURN_ACROSS;
    sc_vector<sc_vector<sc_signal<bool> > > w_VC_SELECTOR_ACROSS;// VC link

    // Trace file for debug proposal (waveform)
    sc_trace_file* tf;

    SC_HAS_PROCESS(SoCIN_Spider);
    SoCIN_Spider(sc_module_name mn);

    const char* moduleName() const { return "SoCIN_Spidergon"; }

    ~SoCIN_Spider();
};

#endif // SOCIN_SPIDER_H
