#include "FlowGenerator.h"
#include <chrono>
#include <ctime>
#include "../Parameters/Parameters.h"

#include "UniformDistribution.h"

// Types of Injection
#include "TypeInjection.h"
#include "ConstantInjection.h"
#include "VarBurstFixInterval.h"
#include "VarIdleFixPacketSize.h"
#include "VarIntervalFixPacketSize.h"
#include "VarPacketSizeFixIdle.h"
#include "VarPacketSizeFixInterval.h"

//#define DEBUG_FG_ADDRESSING

FlowGenerator::FlowGenerator(sc_module_name mn,
                             unsigned short int FG_ID,
                             INoC::TopologyType topologyType,
                             unsigned short numberOfCyclesPerFlit)
    : SoCINModule(mn),
      topologyType(topologyType),
      FG_ID(FG_ID),
      numberCyclesPerFlit(numberOfCyclesPerFlit),
      randomGenerator(std::chrono::system_clock::now().time_since_epoch().count()),
      totalPacketsToSend(0)
{

    // Initialize the default random engine with time seed
    srand(std::time(NULL));

    if(!this->readTrafficFile()) {
        exit(-1);
    }

    vcWidth = (unsigned short) ceil(log2(NUM_VC));
    if(vcWidth>0) {
        o_VC_SEND.init( vcWidth );
    }

    SC_CTHREAD(p_SEND, i_CLK.pos());
    sensitive << i_CLK.pos() << i_RST.pos();

    SC_CTHREAD(p_RECEIVE, i_CLK.pos());
    sensitive << i_CLK.pos() << i_RST.pos();
}

UIntVar FlowGenerator::getHeaderAddresses(unsigned short src,unsigned short dst) {
#ifdef DEBUG_FG_ADDRESSING
    std::cout << "\n[FG] Addressing - Src: " << src << ", Dst: " << dst;
#endif
    UIntVar rib;
    // For Non-Orthogonal and 2D-Orthogonal can be used absolute positions as used in 3D-Orthogonal
    switch ( topologyType ) {
        case INoC::TT_Non_Orthogonal:
            rib.range(RIB_WIDTH*2-1,RIB_WIDTH) = src;
            rib.range(RIB_WIDTH-1,0) = dst;
            break;
        case INoC::TT_Orthogonal2D: {
            unsigned xSrc = ID_TO_COORDINATE_2D_X(src);
            unsigned ySrc = ID_TO_COORDINATE_2D_Y(src);
            unsigned xDst = ID_TO_COORDINATE_2D_X(dst);
            unsigned yDst = ID_TO_COORDINATE_2D_Y(dst);
            rib.range(RIB_WIDTH*2-1,RIB_WIDTH*2-RIB_WIDTH/2) = xSrc;
            rib.range(RIB_WIDTH*2-RIB_WIDTH/2-1,RIB_WIDTH) = ySrc;
            rib.range(RIB_WIDTH-1,RIB_WIDTH/2) = xDst;
            rib.range(RIB_WIDTH/2-1,0) = yDst;
            break;
        }
        case INoC::TT_Orthogonal3D:
            unsigned xSrc = ID_TO_COORDINATE_3D_X(src);
            unsigned ySrc = ID_TO_COORDINATE_3D_Y(src);
            unsigned zSrc = ID_TO_COORDINATE_3D_Z(src);
            unsigned xDst = ID_TO_COORDINATE_3D_X(dst);
            unsigned yDst = ID_TO_COORDINATE_3D_Y(dst);
            unsigned zDst = ID_TO_COORDINATE_3D_Z(dst);
            rib.range(21,19) = xDst;    // TSV - The same X dst because consider that all routers have TSV
            rib.range(18,16) = yDst;    // TSV - The same Y dst ...
            rib.range(15,13) = xSrc;
            rib.range(12,10) = ySrc;
            rib.range( 9, 8) = zSrc;
            rib.range( 7, 5) = xDst;
            rib.range( 4, 2) = yDst;
            rib.range( 1, 0) = zDst;
#ifdef DEBUG_FG_ADDRESSING
            std::cout << "\n   Xsrc: " << xSrc << ", Ysrc: " << ySrc << ", Zsrc: " << zSrc
                      << "\t-\t Xdst: " << xDst << ", Ydst: " << yDst << ", Zdst: " << zDst;
#endif
            break;
    }
#ifdef DEBUG_FG_ADDRESSING
    std::cout << " - RIB: " << rib.to_string(SC_HEX_US,false);
#endif
    return rib;
}

void FlowGenerator::sendFlit(Flit flit, unsigned short virtualChannel) {

    UIntVar v_VC = virtualChannel;
    bool    v_BOP= flit.data[FLIT_WIDTH-2];

    if(v_BOP) {
        for(unsigned short i = 0; i < vcWidth; i++) {
            o_VC_SEND[i].write(v_VC[i]);
        }
    }

    o_WRITE_SEND.write(0);
    o_DATA_SEND.write(flit);
    o_WRITE_SEND.write(1);
    wait();

    while (i_WRITE_OK_SEND.read() == 0) {
        wait();
    }
}

void FlowGenerator::sendPacket(FlowParameters flowParam,
                               unsigned long long cycleToSend,
                               unsigned long payloadLength,
                               unsigned short packetType) {

    if( FG_ID == flowParam.destination ) { // The source and destination addresses are the same
        std::cout << "\n[FlowGenerator] WARNING: The packet source and destination addresses are the same - FG: "
                  << FG_ID << "\n. \t\t Aborting the simulation!" << std::endl;
        sc_stop();
    }

    UIntVar flit(0,FLIT_WIDTH); // Auxiliary variable to build the flit to be sent (FLIT_WIDTH is defined in Parameters.h)

    Packet* packet = new Packet;
    packet->requiredBW = flowParam.required_bw;
    packet->deadline = flowParam.deadline;
    packet->packetCreationCycle = cycleToSend + 1;
    packet->packetId = PARAMS->pckId++;
    packet->payloadLength = payloadLength;
    packet->hops = 0;

    /////////////////// Header ///////////////////
    flit = getHeaderAddresses(FG_ID,flowParam.destination); // Get Addressing according the topology type
    flit[FLIT_WIDTH-2] = 1;                                 // BOP high - Header
    flit.range(CMD_POSITION,CMD_POSITION-1) = packetType;   // Switching (NORMAL, ALLOC, RELEASE, GRANT)
    flit.range(CLS_POS,CLS_POS-2) = flowParam.traffic_class;// Traffic Class
    flit.range(FID_POS,FID_POS-1) = flowParam.flow_id;      // Flow id

    // TODO Verify what virtual channel must be used according the traffic class
    unsigned short virtualChannel = flowParam.traffic_class;

    // It sends the header
    Flit headerFlit;
    headerFlit.data = flit;
    headerFlit.packet_ptr = packet;
    this->sendFlit(headerFlit,virtualChannel); // Send header

    /////////////////// Payload ///////////////////
    for(unsigned short i = 0; i < payloadLength - 1; i++) {
        Flit payload;
        payload.data = i; // The content of the flit is only the number of flit in the packet
        payload.packet_ptr = packet;
        this->sendFlit(payload,virtualChannel); // Sending payload
    }

    /////////////////// Trailer ///////////////////
    // It sends the trailer flit: the lowest word with "Bye" string
//    char msg[4] = "Bye"; // 4 bytes -> [0]: B , [1]: y, [2]: e, [3]: \0
//    //   =   ASCI:  B       |        y       |       e       |   \0
//    flit = ( (msg[0] << 24) | (msg[1] << 16) | (msg[2] << 8) | (msg[3]) );
    // Send trailer with the packet id
    flit = packet->packetId; // Put the packet ID on the trailer payload
    flit[FLIT_WIDTH-1] = 1; // Trailer

    Flit trailer;
    trailer.data = flit;
    trailer.packet_ptr = packet;
    this->sendFlit(trailer, virtualChannel); // Send trailer
}

void FlowGenerator::sendBurst(FlowParameters flowParam, unsigned long long cycleToSend) {

    unsigned int i;
    for ( i = 0; i < flowParam.burst_size-1; i++ ) {
        this->sendPacket(flowParam,cycleToSend, flowParam.payload_length, NORMAL);
        cycleToSend += (flowParam.payload_length+1) * numberCyclesPerFlit;
    }
    if (flowParam.last_payload_length != 0) {
        this->sendPacket(flowParam, cycleToSend, flowParam.last_payload_length, NORMAL);
    }
}


bool FlowGenerator::readTrafficFile() {

    FILE* trafficFile;
    char fgName[20];
    char strTCF[256];
    char str[30];

    unsigned int numberOfFlows;

    /////////////////////////////////////////////
    // READING THE FLOWS FROM THE DESCRIPTOR FILE
    /////////////////////////////////////////////
    // It tries to open the file
    sprintf(strTCF,"%s/%s",WORK_DIR,TRAFFIC_FILENAME);
    if ((trafficFile=fopen(strTCF,"rt")) == NULL) {
        printf("\n[FlowGenerator] ERROR: Impossible to open file \"%s\". Exiting...", strTCF);
        return false;
    } else {
        // It searches for the traffic description in the file
        sprintf(fgName,"tg_%u", FG_ID);
        do {
            fscanf(trafficFile,"%s",str);

            if (!strcmp(fgName,str)){
                // It reads the file in order to determine the number of flows
                fscanf(trafficFile,"%d",&numberOfFlows);
            }
            if (!strcmp(str,"//")) {
                printf("\n\n[FlowGenerator] Traffic generator (%d) has no flow description."
                       "\nIt will be assumed that it has no flow.\t" , FG_ID);
                numberOfFlows = 0;
                break;
            }
        } while ((strcmp(fgName,str)));
    }

    flows.resize(numberOfFlows);
    uniformRandom = std::uniform_int_distribution<int>(0, (int) numberOfFlows -1);

    totalPacketsToSend = 0;
    for(unsigned int flow_index = 0; flow_index < numberOfFlows; flow_index++){
        FlowParameters flow;
        fscanf(trafficFile,"%u" , &(flow.type));
        fscanf(trafficFile,"%hu", &(flow.destination));
        fscanf(trafficFile,"%u" , &(flow.flow_id));
        fscanf(trafficFile,"%u" , &(flow.traffic_class));
        fscanf(trafficFile,"%u" , &(flow.switching_type));
        fscanf(trafficFile,"%lu", &(flow.pck_2send));
        fscanf(trafficFile,"%lu", &(flow.deadline));
        fscanf(trafficFile,"%f" , &(flow.required_bw));
        fscanf(trafficFile,"%u" , &(flow.payload_length));
        fscanf(trafficFile,"%u" , &(flow.idle));
        fscanf(trafficFile,"%u" , &(flow.iat));
        fscanf(trafficFile,"%u" , &(flow.burst_size));
        fscanf(trafficFile,"%u" , &(flow.last_payload_length));
        fscanf(trafficFile,"%f" , &(flow.parameter1));
        fscanf(trafficFile,"%f" , &(flow.parameter2));
        // It determines the total number of packets to be sent by all the flows
        totalPacketsToSend += flow.pck_2send;
        flow.pck_sent = 0;
        flows[flow_index] = flow;
    }

    // It closes the input file
    if (trafficFile != NULL) fclose(trafficFile);

    return true;
}

FlowGenerator::FlowParameters& FlowGenerator::getFlow() {

    unsigned int flowIndex = 0;
    FlowParameters* flowSelected = NULL;

    bool hasFlow = false;

    // Verify if there are some flow with packets to send
    for( unsigned int i = 0; i < flows.size(); i++ ) {
        flowSelected = &flows[i];
        if( flowSelected->pck_sent < flowSelected->pck_2send ) {
            hasFlow = true;
            break;
        }
    }

    if( !hasFlow ) {
        std::cout << std::endl << "FG " << FG_ID << " has no flow with packets to send!";
    }

    // It randomly chooses one of the flows that still has some packet to send
    do {
        flowIndex = uniformRandom(randomGenerator);
        flowSelected = &this->flows[flowIndex];
    } while( flowSelected->pck_sent == flowSelected->pck_2send );

    return *flowSelected;
}

void FlowGenerator::reloadFlows() {
    for(unsigned int i = 0; i < flows.size(); i++) {
        FlowParameters* flow = &flows[i];
        flow->pck_sent = 0;
    }
}

void FlowGenerator::p_SEND() {

    // Reseting
    Flit fNull = 0;
    o_DATA_SEND.write(fNull);
    o_WRITE_SEND.write(0);
    o_END_OF_TRANSMISSION.write(0);
    o_NUMBER_OF_PACKETS_SENT.write(0);
    wait();
//        // TEMP
//        destGen = new UniformDistribution;

    unsigned int numFlows = this->flows.size();
    if(numFlows > 0) {
        unsigned long long cycleToSendNextPacket;
        // It determines the cycle to send the first packet
        cycleToSendNextPacket = i_CLK_CYCLES.read();

        unsigned long long packetsSent = 0;

        ///// Sending the packets /////
        while( true ) { // Send packets
            FlowParameters& flow = this->getFlow(); // Get a flow randomly

            TypeInjection* tpInjection = NULL;
            switch (flow.type) {
                case 0: // Constant
                    tpInjection = new ConstantInjection(numberCyclesPerFlit);
                    break;
                case 1: // Variable idle time - Fix message size
                    tpInjection = new VariableIdleFixPacketSize(numberCyclesPerFlit);
                    break;
                case 2: // Variable message size - Fix idle time
                    tpInjection = new VarPacketSizeFixIdle(numberCyclesPerFlit);
                    break;
                case 3: // Variable message size - Fix message inteval
                    tpInjection = new VarPacketSizeFixInterval(numberCyclesPerFlit);
                    break;
                case 4: // Variable message interval - Fix message size
                    tpInjection = new VarIntervalFixPacketSize(numberCyclesPerFlit);
                    break;
                case 5: // Variable burst size - Fix message interval
                    tpInjection = new VarBurstFixInterval(numberCyclesPerFlit);
                    break;
                default: break;
            }

            // PARETO-based generation
            if(flow.type > 0 && flow.type <= 5) {
                do {
                    float r    = ((float) (rand()%10000)) / (10000.0);
                    float ton  = pow( (float)(1-r),(-1.0/flow.parameter1) );
                    float toff = pow( (float)(1-r),(-1.0/flow.parameter2) );
            // If function of probability is Pareto, it determines the required bw
                    flow.required_bw = ton/(ton+toff);
                    tpInjection->adjustFlow(flow);
                } while (flow.payload_length == 0);
            }

            /////////////////////////////////////////////////////////////////
            // EMULATING PROCESSING (IDLE INTERVAL BEFORE INJECTING A PACKET)
            /////////////////////////////////////////////////////////////////
            // It updates the cycle to send the next packet adding the idle cycles of the
            // current flow to the value previously calculated value. Then, it inserts
            // wait cycles until cycle_to_send_next_pck is reached
            cycleToSendNextPacket += flow.idle;
            if( cycleToSendNextPacket < i_CLK_CYCLES.read() ) {
                std::cout << std::endl << "FG " << FG_ID << " under congestion to send packets.";
            }
// ZEFERINO
            while ( i_CLK_CYCLES.read() < cycleToSendNextPacket) wait(); // Wait until the cycle to send the packet


            /////////////////////
            // SENDING THE PACKET
            /////////////////////
            // SENDING PACKETS IN A BURST
            if (flow.burst_size != 0) {
                // It sends a burst of packets
                this->sendBurst(flow,cycleToSendNextPacket);

                // It increments the packet counters and calculates when the first packet
                // of the next burst of packets have to be injected. But, if last_payload_length
                // equals 0, the last packet is not taken into account because it was not
                // actually sent
                if (flow.last_payload_length != 0) {
                    // It increments the packet counters
                    o_NUMBER_OF_PACKETS_SENT.write( o_NUMBER_OF_PACKETS_SENT.read() + flow.burst_size );
                    flow.pck_sent += flow.burst_size;

                    // It calculates when the first packet of the next burst of packets have to be injected
                    cycleToSendNextPacket += ((flow.payload_length+HEADER_LENGTH) * numberCyclesPerFlit * (flow.burst_size - 1))
                            + ((flow.last_payload_length+HEADER_LENGTH) * numberCyclesPerFlit);
//ZEFERINO                            + flow[flow_index].idle;
                } else {
                    // It increments the packet counters
                    o_NUMBER_OF_PACKETS_SENT.write( o_NUMBER_OF_PACKETS_SENT.read() + flow.burst_size - 1);
                    flow.pck_sent += flow.burst_size - 1;

                    // It calculates when the first packet of the next burst of packets have to be injected
                    cycleToSendNextPacket += ((flow.payload_length+HEADER_LENGTH) * numberCyclesPerFlit * (flow.burst_size - 1));
//ZEFERINO                            + flow[flow_index].idle;
                }

            // SENDING PACKETS ONE BY ONE (NOT IN A BURST)
            } else {
                unsigned short pckType = NORMAL; // By default - NORMAL packet - WORMHOLE uses normal
                // It sends the packet
                switch (flow.switching_type) {
                    case CS :
                        if (flow.pck_sent == 0) {
                            // If it is first packet, it alocates the circuit
                            pckType = ALOC;
                        } else {
                            if (flow.pck_sent == (flow.pck_2send-1)) {
                                // If it is last packet, it releases the circuit
                                pckType = RELEASE;
                            } else {
                                // If its not the first and neither the last packets, send it as a normal packet
                                pckType = NORMAL;
                            }
                        }
                    break;

                default : break;
                }
                this->sendPacket(flow,cycleToSendNextPacket,flow.payload_length,pckType);

                // It increments the packet counters
                o_NUMBER_OF_PACKETS_SENT.write( o_NUMBER_OF_PACKETS_SENT.read() + 1);
                flow.pck_sent++;

                // It calculates when the next packet have to be injected
                cycleToSendNextPacket += ((flow.payload_length+HEADER_LENGTH) * numberCyclesPerFlit);
//ZEFERINO                        + flow[flow_index].idle;
            }
            // Deallocates the channel after send a packet or a burst
            o_WRITE_SEND.write(0);
            o_DATA_SEND.write(fNull);

            // It increments the number of total packets sent
            if (flow.burst_size != 0)
                packetsSent += flow.burst_size;
            else
                packetsSent++;

// EDUARDO - Sending packets forever
            if( packetsSent % totalPacketsToSend == 0 ) { // Old stop condition - remove to back the old model
                if( stopMethod != StopSim::AllPacketsDelivered ) {
                    this->reloadFlows();
                } else {
                    break;
                }
            }
// EDUARDO - Sendind packets forever


// ZEFERINO
//            // It inserts wait states until cycle_to_inject is reached
//            while(clock_cycles.read() < cycle_to_send_next_pck) wait();
        }
    }

//    if( FG_ID == 0 ) {
//        FlowParameters flow;
//        flow.type = 0;
//        flow.burst_size = 0;
//        flow.deadline = 0;
//        flow.destination = 0; // Invalid for FG 0
//        flow.flow_id = 0;
//        flow.iat = 0;
//        flow.idle = 0;
//        flow.last_payload_length = 0;
//        flow.parameter1 = 0;
//        flow.parameter2 = 0;
//        flow.payload_length = 1;
//        flow.pck_2send = 10;
//        flow.pck_sent = 0;
//        flow.required_bw = 320;
//        flow.switching_type = 0;
//        flow.traffic_class = 0;

//        for( unsigned int i = 0; i < flow.pck_2send; i++) {
//            flow.destination = destGen->getDestination(FG_ID);
//            this->sendPacket(flow,0,flow.payload_length,NORMAL);
//            o_WRITE_SEND.write(0);
//            o_DATA_SEND.write(fNull);

//            for( int x = 0; x < 10; x++) {
//                wait();
//            }
//        }
//    }

    o_END_OF_TRANSMISSION.write(1);
    wait();

}

void FlowGenerator::p_RECEIVE() {
    o_READ_RECEIVE.write(1);
    o_NUMBER_OF_PACKETS_RECEIVED.write(0);
    wait();

    UIntVar data;
    bool trailer;
//    bool header;
    Flit f;
    while(1) {
        f = i_DATA_RECEIVE.read();
        data = f.data;
        trailer = data[FLIT_WIDTH-1];
//        header = data[FLIT_WIDTH-2];

        if ((i_READ_OK_RECEIVE.read()==1) && trailer) {
            o_NUMBER_OF_PACKETS_RECEIVED.write(o_NUMBER_OF_PACKETS_RECEIVED.read() + 1);
//            std::cout << "\nFG " << FG_ID << " - received: " << number_of_packets_received << " @ " << sc_time_stamp();
        }
        wait();
    }
}
