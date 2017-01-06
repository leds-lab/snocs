////////////////////////////////////////////////////////////////////////////////
// 
// name         fg.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __FG_H__
#define __FG_H__

#include <systemc.h>
#include "../SoCINDefines.h"

#include "../NoC/NoC.h"

#ifdef RIB_WIDTH
#undef RIB_WIDTH
#define RIB_WIDTH 8
#endif


SC_MODULE(fg)
{
    INoC::TopologyType topologyType;

    unsigned short vcWidth;
    // INTERFACE
    // System signals
    sc_in<bool>               clk;
    sc_in<bool>               rst;
    sc_in<unsigned long> clock_cycles;

    // Interface to the VCs of the communication port (its is done by means of FIFOs)
    sc_out<Flit>  snd_data;	// I/F with the output FIFO
    sc_out<bool>  snd_wr;
    sc_in<bool>   snd_wok;
    sc_in<Flit>   rcv_data; // I/F with the input FIFO
    sc_in<bool>   rcv_rok;
    sc_out<bool>  rcv_rd;
    sc_vector<sc_out<bool> > o_VC; // Virtual channel

    // Status signals
    sc_out<bool>                  eot;
    sc_out<unsigned int>          number_of_packets_sent;
    sc_out<unsigned int>          number_of_packets_received;

    // Internal data structures
    unsigned short int FG_ID;

    // Data structures with the basic fields of a flow
    struct FLOW_TYPE {
        unsigned int  type;                //  0: 0 = gtr determines the traffic model, 1 = tg determines the traffic model by using PARETO
        unsigned short destination;
        unsigned int  flow_id;             //  3: Flow identifier
        unsigned int  traffic_class;       //  4: Class of traffic (RT0, RT1, nRT0, nRT1)
        unsigned long pck_2send;           //  5: Number of packets to be sent by the flow
        unsigned long deadline;            //  6: Required deadline to deliver the message
        float         required_bw;         //  7: A percentual value of the channel bandwidth (e.g. 0.2 = 20%)
        unsigned int  payload_length;      //  8: Number of flits in the payload (including the trailer)
        unsigned int  idle;                //  9: Number of idle cycles between two packets
        unsigned int  iat;                 // 10: Inter-arrival time
        unsigned int  burst_size;          // 11: Number of packets in a burst transmission
        unsigned int  last_payload_length; // 12: Number of flits in the payload (including the trailer) of the last packet in a burst
        float         parameter1;          // 13: Additional parameter for TG-based trafic modelling (e.g.alfa_on in Pareto)
        float         parameter2;          // 14: Additional parameter for TG-based trafic modelling (e.g.alfa_off in Pareto)
        unsigned int  switching_type;      // 15: Switching type
        unsigned long pck_sent;            // 16: Status about the number of packets already sent
    };

    // Module process
    void p_send();
    void p_receive();

    // Member functions
    void f_send_flit(Flit , unsigned int);
    void f_send_packet(unsigned short nodeId, unsigned long long, FLOW_TYPE, unsigned long long, unsigned int);
    void f_send_burst_of_packets(unsigned short nodeId , unsigned long long, FLOW_TYPE);

    UIntVar getHeaderAddresses(unsigned short src, unsigned short destination);

    unsigned short nb_cycles_per_flit;

    SC_HAS_PROCESS(fg);
    //////////////////////////////////////////////////////////////////////////////
    fg(sc_module_name nm,
       unsigned short int FG_ID,
       INoC::TopologyType topologyType,
       unsigned short nb_cycles_per_flit) :
        sc_module(nm),
        topologyType(topologyType),
        FG_ID(FG_ID),
        nb_cycles_per_flit(nb_cycles_per_flit)
    //////////////////////////////////////////////////////////////////////////////
    {
        std::cout << "\nInit FG: " << FG_ID;
        vcWidth = (unsigned short) ceil(log2(NUM_VC));
        if(vcWidth>0) {
            o_VC.init( vcWidth );
        }
        SC_CTHREAD(p_send, clk.pos());
        sensitive << clk.pos() << rst.pos();

        SC_CTHREAD(p_receive, clk.pos());
        sensitive << clk.pos() << rst.pos();
    }

};
#endif // __FG_H__
