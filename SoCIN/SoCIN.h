#ifndef __SOCIN_H__
#define __SOCIN_H__

#include "../src/NoC.h"

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// SoCINfp with virtual channels /////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
class SoCINfp_VC : public INoC_VC {
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

    void p_DEBUG();

    sc_trace_file* tf;

    SC_HAS_PROCESS(SoCINfp_VC);
    SoCINfp_VC(sc_module_name mn);

    const char* moduleName() const { return "SoCINfp_VC"; }
    TopologyType topologyType() const { return INoC::TT_Orthogonal2D; }
    ~SoCINfp_VC();
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// SoCINfp ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
class SoCINfp : public INoC {
public:

    // SIGNALS
    // Signals for the links on the X direction
    sc_vector<sc_signal<Flit> > w_X_DATA_TO_LEFT;
    sc_vector<sc_signal<bool> > w_X_VALID_TO_LEFT;
    sc_vector<sc_signal<bool> > w_X_RETURN_TO_LEFT;
    sc_vector<sc_signal<Flit> > w_X_DATA_TO_RIGHT;
    sc_vector<sc_signal<bool> > w_X_VALID_TO_RIGHT;
    sc_vector<sc_signal<bool> > w_X_RETURN_TO_RIGHT;

    // Signals for the links on the Y direction
    sc_vector<sc_signal<Flit> > w_Y_DATA_TO_SOUTH;
    sc_vector<sc_signal<bool> > w_Y_VALID_TO_SOUTH;
    sc_vector<sc_signal<bool> > w_Y_RETURN_TO_SOUTH;
    sc_vector<sc_signal<Flit> > w_Y_DATA_TO_NORTH;
    sc_vector<sc_signal<bool> > w_Y_VALID_TO_NORTH;
    sc_vector<sc_signal<bool> > w_Y_RETURN_TO_NORTH;

    void p_DEBUG();

    sc_trace_file* tf;

    SC_HAS_PROCESS(SoCINfp);
    SoCINfp(sc_module_name mn);

    const char* moduleName() const { return "SoCINfp"; }
    TopologyType topologyType() const { return INoC::TT_Orthogonal2D; }

    ~SoCINfp();
};

#endif // __SOCIN_H__
