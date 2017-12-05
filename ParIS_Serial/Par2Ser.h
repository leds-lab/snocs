#ifndef __PAR2SER_H__
#define __PAR2SER_H__

#include "../SoCINModule.h"
#include "../SoCINDefines.h"

class Par2Ser : public SoCINModule {
public:

    // Interface
    sc_in<bool>  i_CLK;
    sc_in<bool>  i_RST;
    sc_in<bool>  i_VALID;
    sc_in<bool>  i_RETURN;
    sc_in<Flit>  i_DATA;
    sc_out<bool> o_VALID;
    sc_out<bool> o_RETURN;
    sc_out<bool> o_DATA;

    // FSM States
    enum StateType { s_S0, s_S1 };

    // Internal's structures
    sc_signal<StateType>      r_CURRENT;    // Current state of Serializer FSM
    sc_signal<StateType>      r_NEXT;       // Next state of Serializer FSM
    sc_signal<unsigned short> r_BIT_POS;    // Bit position being read from the parallel data received
    sc_signal<Flit>           r_DATA;       // Register to store the data to be transfered

    // Processes
    void p_CURRENT_STATE();
    void p_NEXT_STATE();
    void p_COUNT();
    void p_OUTDATA();
    void p_OUTVALID();
    void p_OUTRETURN();

    SC_HAS_PROCESS(Par2Ser);
    Par2Ser(sc_module_name mn);

    const char* moduleName() const { return "Parallel_To_Serial"; }
    ModuleType moduleType() const { return SoCINModule::OtherT; }

    ~Par2Ser() {}
};

#endif // PAR2SER_H
