////////////////////////////////////////////////////////////////////////////////
// 
// name         tm.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TM_SINGLE_H__
#define __TM_SINGLE_H__

#include <systemc.h>
#include "../SoCINDefines.h"

SC_MODULE(tm_single)
{

    unsigned short vcWidth;
    // INTERFACE
    // System signals
    sc_in_clk                 clk;
    sc_in<bool>               rst;
    sc_in<bool>               eos;
    sc_in<unsigned long long> clock_cycles;

    sc_vector<sc_in<bool> > i_VC_SEL;

    // Communication Port
    sc_in<Flit> data;
    sc_in<bool> val;
    sc_in<bool> ret;

    // Internal data structures
    unsigned short int XID, YID;
    char *PATH;
    char *FILENAME;


    // Variables
    unsigned int pck_received;
    unsigned int pck_size;
    unsigned int nb_of_cycles_per_flit;
    unsigned int x_src;
    unsigned int y_src;
    unsigned int x_dest;
    unsigned int y_dest;
    unsigned int flow_id;
    unsigned int traffic_class;
    /*
  unsigned long int header;
  unsigned long int deadline;
  unsigned long int cycle_to_send;
  unsigned long int cycle_of_arriving;
    */
    unsigned long long header;
    unsigned long long deadline;
    unsigned long long cycle_to_send;
    unsigned long long cycle_of_arriving;


    //  unsigned long int cycle_of_injection;
    unsigned int required_bw;
    //  unsigned long int delay_of_injection;
    //  unsigned long int latency;
    //  double latency;

    char path_filename[256];
    FILE *fp_out;

    // Module process
    void p_probe();
    void p_stop();

    // Member functions
    void f_write_received_flit(FILE *);


    SC_HAS_PROCESS(tm_single);

    //////////////////////////////////////////////////////////////////////////////
    tm_single(sc_module_name nm,
              unsigned short int XID,
              unsigned short int YID,
              char *PATH,
              char *FILENAME) :
        sc_module(nm),
        XID(XID),
        YID(YID),
        PATH(PATH),
        FILENAME(FILENAME)
      //////////////////////////////////////////////////////////////////////////////
    {
        vcWidth = (unsigned short) ceil(log2(NUM_VC));
        i_VC_SEL.init(vcWidth);

        SC_CTHREAD(p_probe, clk.pos());
        sensitive << clk.pos() << rst.pos();

        SC_METHOD(p_stop);
        sensitive << eos.pos();
    }

    ~tm_single() {
        delete[] FILENAME;
    }

};
#endif // __TM_SINGLE_H__
