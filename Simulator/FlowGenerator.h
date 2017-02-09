#ifndef __FLOWGENERATOR_H__
#define __FLOWGENERATOR_H__

#include "../SoCINDefines.h"
#include "../NoC/NoC.h"

#include "DestinationGenerator.h"
#include "../StopSim/StopSim.h"
#include "UnboundedFifo.h"

// Switching types
#define WH 0
#define CS 1

// Switching command
#define NORMAL  0
#define ALOC    1
#define RELEASE 2
#define GRANT   3

#define HEADER_LENGTH 1

#define TRAFFIC_FILENAME "traffic.tcf"

class FlowGenerator : public SoCINModule  {
public:
    StopSim::StopMethod stopMethod;

    UnboundedFifo* u_FIFO;

    struct FlowParameters {             // Manage the traffic parameters
        unsigned int  type;                 //  0: 0 = gtr determines the traffic model, 1 = tg determines the traffic model by using PARETO
        unsigned short destination;         //  1: Destination
        unsigned int  flow_id;              //  2: Flow identifier
        unsigned int  traffic_class;        //  3: Class of traffic (RT0, RT1, nRT0, nRT1)
        unsigned long pck_2send;            //  4: Number of packets to be sent by the flow
        unsigned long deadline;             //  5: Required deadline to deliver the message
        float         required_bw;          //  6: A percentual value of the channel bandwidth (e.g. 0.2 = 20%)
        unsigned int  payload_length;       //  7: Number of flits in the payload (including the trailer)
        unsigned int  idle;                 //  8: Number of idle cycles between two packets
        unsigned int  iat;                  //  9: Inter-arrival time
        unsigned int  burst_size;           // 10: Number of packets in a burst transmission
        unsigned int  last_payload_length;  // 11: Number of flits in the payload (including the trailer) of the last packet in a burst
        float         parameter1;           // 12: Additional parameter for TG-based trafic modelling (e.g.alfa_on in Pareto)
        float         parameter2;           // 13: Additional parameter for TG-based trafic modelling (e.g.alfa_off in Pareto)
        unsigned int  switching_type;       // 14: Switching type
        unsigned long pck_sent;             // 15: Status about the number of packets already sent
    };

    // INTERFACE
    // System signals
    sc_in<bool>          i_CLK;
    sc_in<bool>          i_RST;
    sc_in<unsigned long long> i_CLK_CYCLES;

    // Interface to the VCs of the communication port (its is done by means of FIFOs)
    sc_out<Flit>  o_DATA_SEND;          // I/F with the output FIFO
    sc_out<bool>  o_WRITE_SEND;
    sc_in<bool>   i_WRITE_OK_SEND;
    sc_vector<sc_out<bool> > o_VC_SEND; // Virtual channel selector
    sc_in<Flit>   i_DATA_RECEIVE;       // I/F with the input FIFO
    sc_in<bool>   i_READ_OK_RECEIVE;
    sc_out<bool>  o_READ_RECEIVE;

    // Status signals
    sc_out<bool>         o_END_OF_TRANSMISSION;
    sc_out<unsigned int> o_NUMBER_OF_PACKETS_SENT;
    sc_out<unsigned int> o_NUMBER_OF_PACKETS_RECEIVED;


    // TEST
    sc_in<bool> i_UNBOUNDEDFIFO_NOTEMPTY;

    // Module's processes
    void p_SEND();
    void p_RECEIVE();

    // Auxiliar functions
    UIntVar getHeaderAddresses(unsigned short src, unsigned short destination);
    void sendFlit(Flit flit, unsigned short virtualChannel);
    void sendPacket(FlowParameters flowParam, unsigned long long cycleToSend,
                    unsigned long payloadLength, unsigned short packetType);
    void sendBurst(FlowParameters flowParam, unsigned long long cycleToSend);

    bool readTrafficFile();
    void reloadFlows();

    SC_HAS_PROCESS(FlowGenerator);
    FlowGenerator(sc_module_name mn,
                  unsigned short int FG_ID,
                  INoC::TopologyType topologyType,
                  unsigned short numberOfCyclesPerFlit);

    ModuleType moduleType() const { return SoCINModule::TFlowGenerator; }
    const char* moduleName() const { return "FlowGenerator"; }

    ~FlowGenerator() {}

    inline unsigned long getTotalPacketsToSend() const { return this->totalPacketsToSend; }

protected:
    virtual FlowParameters &getFlow();

private:
    INoC::TopologyType topologyType;    // Topology type to identify the header addressing format
    unsigned short FG_ID;               // Flow generator identifier
    unsigned short numberCyclesPerFlit; // Number of cycles to transmit a flit in the link on the flow control used
    unsigned short vcWidth;             // Virtual channel selector width
    std::vector<FlowParameters> flows;  // Flows from the descriptor on the traffic configuration file

    std::default_random_engine randomGenerator;
    std::uniform_int_distribution<int> uniformRandom;

    unsigned long totalPacketsToSend;

    DestinationGenerator* destGen;
};

#endif // __FLOWGENERATOR_H__
