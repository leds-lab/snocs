#ifndef __SER2PAR_H__
#define __SER2PAR_H__

#include "../SoCINDefines.h"
#include "../SoCINModule.h"

class Ser2Par : public SoCINModule {
protected:
    unsigned short router;
    unsigned short portNumber;
public:
    Packet* pck_ptr;

    // Interface
    sc_in<bool>  i_CLK;
    sc_in<bool>  i_RST;
    sc_in<bool>  i_VALID;
    sc_in<bool>  i_RETURN;
    sc_in<bool>  i_DATA;
    sc_out<bool> o_VALID;
    sc_out<bool> o_RETURN;
    sc_out<Flit> o_DATA;

    sc_in<Packet *> _packet_reference;

    // FSM states
    enum StateType { s_S0=0, s_S1, s_S2 };

    // Internal's structures
    sc_signal<StateType>      r_CURRENT;    // Current state of Serializer FSM
    sc_signal<StateType>      r_NEXT;       // Next state of Serializer FSM
    sc_signal<Flit>           r_DATA;       // Register to store the data to be transfered
    sc_signal<unsigned short> r_BIT_POS;    // Bit position being read to assembly the output

    // Processes
    void p_CURRENT_STATE();
    void p_NEXT_STATE();
    void p_MOUNT();
    void p_OUTDATA();
    void p_OUTVALID();
    void p_OUTRETURN();

    sc_trace_file* tf;

    SC_HAS_PROCESS(Ser2Par);
    Ser2Par(sc_module_name mn, unsigned short router, unsigned short portNumber);

    const char* moduleName() const { return "Serial_To_Parallel"; }
    ModuleType moduleType() const { return SoCINModule::OtherT; }

    ~Ser2Par();
};

#endif // SER2PAR_H
