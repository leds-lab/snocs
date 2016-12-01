////////////////////////////////////////////////////////////////////////////////
//
// name         fg.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <random>

#include "fg.h"
#define FILENAME  "traffic.tcf"
#define VAR_IDL_FXD_PCK 1
#define VAR_PCK_FXD_IDL 2
#define VAR_PCK_FXD_IAT 3
#define VAR_IAT_FXD_PCK 4
#define VAR_BST_FXD_IAT 5

#define HEADER_LENGTH 1

// Switching types
#define WH 0
#define CS 1

// Switching command
#define NORMAL  0
#define ALOC    1
#define RELEASE 2
#define GRANT   3

//#define DEBUG_FG_ADDRESSING

UIntVar fg::getHeaderAddresses(unsigned short src,unsigned short dst) {

    UIntVar rib;
#ifdef DEBUG_FG_ADDRESSING
    std::cout << "\n[FG] Addressing - Src: " << src << ", Dst: " << dst;
#endif
    // Can be used absolute positions as 3D
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

////////////////////////////////////////////////////////////////////////////////
void fg::f_send_flit(Flit flit, unsigned int traffic_class)
////////////////////////////////////////////////////////////////////////////////
{
    // TODO: Verificar como será o tratamento caso haja mais classes de tráfego do que VCs
    UIntVar v_VC = traffic_class;
    bool    v_BOP= flit.data[FLIT_WIDTH-2];

    if(v_BOP) {
        for(unsigned short i = 0; i < vcWidth; i++) {
            o_VC[i].write(v_VC[i]);
        }
    }

    snd_wr.write(0);
    snd_data.write(flit);
    snd_wr.write(1);
    wait();

    while (snd_wok.read() == 0) {
        wait();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void fg::f_send_packet(unsigned short nodeId, unsigned long long cycle_to_send, FLOW_TYPE flow,
                       unsigned long long payload_length, unsigned int packet_type)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    if ( (FLIT_WIDTH-2) < (2*RIB_WIDTH + 7)) {
        printf("\n\t[fg.cpp] ERROR: Data channel width should be greater or equal to %d bits\t",2*RIB_WIDTH + 7);
        exit(1);
    }

    if( nodeId == flow.destination ) { // The destination is the same as source address
        std::cout << "\n[FlowGenerator] WARNING: The packet source and destination are the same - FG: "
                  << FG_ID << "\n. \t\t Aborting the simulation!" << std::endl;
        sc_stop();
    }

    UIntVar flit(0,FLIT_WIDTH); // Auxiliary variable to build the flit to be sent (FLIT_WIDTH is defined in Parameters.h)

    Packet* packet = new Packet;
    packet->requiredBW = flow.required_bw;
    packet->deadline = flow.deadline;
    packet->packetCreationCycle = cycle_to_send + 1;
    packet->packetId = PARAMS->pckId++;
    packet->payloadLength = payload_length;

    /////////////////// Header ///////////////////
    flit = getHeaderAddresses(nodeId,flow.destination);    // Get Addressing according the topology type
    flit[FLIT_WIDTH-2] = 1;                                // BOP high - Header
    flit.range(CMD_POSITION,CMD_POSITION-1) = packet_type; // Switching (NORMAL, ALLOC, RELEASE, GRANT)
    flit.range(CLS_POS,CLS_POS-2) = flow.traffic_class;    // Traffic Class
    flit.range(FID_POS,FID_POS-1) = flow.flow_id;          // Flow id

    // It sends the header
    Flit headerFlit;
    headerFlit.data = flit;
    headerFlit.packet_ptr = packet;
    f_send_flit(headerFlit, flow.traffic_class); // Send header

    /////////////////// Payload ///////////////////
    for(unsigned short i = 0; i < payload_length-1; i++) {
        Flit payload;
        payload.data = i; // The content of the flit is only the number of flit in the packet
        payload.packet_ptr = packet;
        f_send_flit(payload,flow.traffic_class);
    }

    /////////////////// Trailer ///////////////////
    // It sends the trailer flit: the lowest word with "Bye" string
    char msg[4] = "Bye"; // 4 bytes -> [0]: B , [1]: y, [2]: e, [3]: \0
    //   =   ASCI:  B       |        y       |       e       |   \0
    flit = ( (msg[0] << 24) | (msg[1] << 16) | (msg[2] << 8) | (msg[3]) );
    flit[FLIT_WIDTH-1] = 1; // Trailer

    Flit trailer;
    trailer.data = flit;
    trailer.packet_ptr = packet;
    f_send_flit(trailer, flow.traffic_class); // Send trailer

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void fg::f_send_burst_of_packets(unsigned short nodeId, unsigned long long cycle_to_send, FLOW_TYPE flow)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    unsigned int i;

    for (i=0; i<flow.burst_size-1;i++) {
        f_send_packet(nodeId, cycle_to_send, flow, flow.payload_length, NORMAL);
        cycle_to_send += (flow.payload_length+HEADER_LENGTH) * nb_cycles_per_flit;
    }
    if (flow.last_payload_length!=0)
        f_send_packet(nodeId, cycle_to_send, flow, flow.last_payload_length, NORMAL);
}

////////////////////////////////////////////////////////////////////////////////
void fg::p_send()
////////////////////////////////////////////////////////////////////////////////
{
    unsigned int i;//,j,k;

    // Variables
    sc_uint<RIB_WIDTH>    id;

    unsigned int nb_of_flows;
    unsigned int flow_index;
    unsigned long cycle_to_send_next_pck; // When the next packet has to be injected
    unsigned long long total_pck_2send;
    unsigned long long pck_counter;
    unsigned int packet_type;

    float r;
    float ton, toff, tmp_float;  // variables used for Pareto-based generation

    char fg_name[30];
    char str[30];

    FILE *fp_in;
    FLOW_TYPE *flow;


    /////////////////////////////////////////////
    // READING THE FLOWS FROM THE DESCRIPTOR FILE
    /////////////////////////////////////////////

    // It tries to open the file
    char strTCF[256];
    sprintf(strTCF,"%s/%s",WORK_DIR,FILENAME);
    if ((fp_in=fopen(strTCF,"rt")) == NULL) {
        printf("\n\t[fg.cpp] ERROR: Impossible to open file \"%s\". Exiting...", strTCF);
        exit(1);
    } else {
        // It searches for the traffic description in the file
        sprintf(fg_name,"tg_%u", FG_ID);
        do {
            fscanf(fp_in,"%s",str);

            if (!strcmp(fg_name,str)){
                // It reads the file in order to determine the number of flows
                fscanf(fp_in,"%d",&nb_of_flows);
            }
            if (!strcmp(str,"//")) {
                printf("\n\n[fg.cpp] Traffic generator (%d) has no flow description. It will be assumed that it has no flow.\t" , FG_ID);
                nb_of_flows = 0;
                break;
            }
        } while ((strcmp(fg_name,str)));

    }

    // It allocates memory to store the data structure of each flow
    if (nb_of_flows) {
        if ((flow = (FLOW_TYPE*) malloc(sizeof(FLOW_TYPE)*nb_of_flows))==NULL) {
            printf("\n\n\tERROR: There is not enough memory to create the flows for core %d !!!\n\n", FG_ID);
            exit(1);
        }

        for(flow_index = 0; flow_index < nb_of_flows; flow_index++){
            fscanf(fp_in,"%u" , &(flow[flow_index].type));
//            fscanf(fp_in,"%u" , &(flow[flow_index].x_dest));
//            fscanf(fp_in,"%u" , &(flow[flow_index].y_dest));
            fscanf(fp_in,"%hu" , &(flow[flow_index].destination));
            fscanf(fp_in,"%u" , &(flow[flow_index].flow_id));
            fscanf(fp_in,"%u" , &(flow[flow_index].traffic_class));
            fscanf(fp_in,"%u" , &(flow[flow_index].switching_type));
            fscanf(fp_in,"%lu", &(flow[flow_index].pck_2send));
            fscanf(fp_in,"%lu", &(flow[flow_index].deadline));
            fscanf(fp_in,"%f" , &(flow[flow_index].required_bw));
            fscanf(fp_in,"%u" , &(flow[flow_index].payload_length));
            fscanf(fp_in,"%u" , &(flow[flow_index].idle));
            fscanf(fp_in,"%u" , &(flow[flow_index].iat));
            fscanf(fp_in,"%u" , &(flow[flow_index].burst_size));
            fscanf(fp_in,"%u" , &(flow[flow_index].last_payload_length));
            fscanf(fp_in,"%f" , &(flow[flow_index].parameter1));
            fscanf(fp_in,"%f" , &(flow[flow_index].parameter2));
            flow[flow_index].pck_sent = 0;

#ifdef DEBUG
            printf("\n\n\tFlow  %02d : ",flow_index);
            printf("x_dest               = %u\t",flow[flow_index].x_dest);
            printf("\n\t\t   y_dest               = %u\t"  , flow[flow_index].y_dest);
            printf("\n\t\t   flow_id              = %u\t"  , flow[flow_index].flow_id);
            printf("\n\t\t   traffic_class        = %u\t"  , flow[flow_index].traffic_class);
            printf("\n\t\t   switching_type  			= %u\t"  , flow[flow_index].switching_type);
            printf("\n\t\t   pck_2send            = %llu\t", flow[flow_index].pck_2send);
            printf("\n\t\t   deadline	            = %lu\t" , flow[flow_index].deadline);
            printf("\n\t\t   required_bw          = %.1f\t", flow[flow_index].required_bw);
            printf("\n\t\t   payload_length       = %u\t"  , flow[flow_index].payload_length);
            printf("\n\t\t   idle          				= %u\t"  , flow[flow_index].idle);
            printf("\n\t\t   iat                  = %u\t"  , flow[flow_index].iat);
            printf("\n\t\t   burst_size           = %u\t"  , flow[flow_index].burst_size);
            printf("\n\t\t   last_payload_length  = %u\t"  , flow[flow_index].last_payload_length);

#endif
        }
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
//    srand(time(NULL));

    // It closes the input file
    if (fp_in != NULL) fclose(fp_in);

    // Reseting
    Flit fNull = 0;
    snd_data.write(fNull);
    snd_wr.write(0);
    eot.write(0);
    number_of_packets_sent.write(0x0);
    wait();

    //////////////////
    // FLOW GENERATION
    //////////////////
    if (nb_of_flows) {
        // It determines the router id (network address)
        id = FG_ID;

        // It determines the total number of packets to be sent by all the flows
        for(total_pck_2send = 0, i=0; i<nb_of_flows; i++)
            total_pck_2send += (unsigned long long) flow[i].pck_2send;

        // It determines the cycle to send the first packet
        cycle_to_send_next_pck = clock_cycles.read();

        // It sends all the packets
        pck_counter = 0;
        while (pck_counter < total_pck_2send) {
            // It randomly chooses one of the flows that still has some packet to send
            do {
//                std::uniform_int_distribution<int> distribution(0,nb_of_flows);
                flow_index = rand() % (nb_of_flows);
//                flow_index = distribution(generator);
            } while (flow[flow_index].pck_sent == flow[flow_index].pck_2send);
//            printf("\nFluxo Selecionado: %u",flow_index);

            // PARETO-based generation
            // If function of probability is Pareto, it determines the required bw
            if (flow[flow_index].type != 0) {
                do {
                    r = ((float)(rand()%10000)) / (10000.0);
                    ton  = pow( (float)(1-r),(-1.0/flow[flow_index].parameter1) );
                    toff = pow( (float)(1-r),(-1.0/flow[flow_index].parameter2) );
                    flow[flow_index].required_bw = ton/(ton+toff);

                    switch(flow[flow_index].type) {
                        case VAR_IDL_FXD_PCK: // It determines the number of idle cycles
                            tmp_float = (1.0/flow[flow_index].required_bw - 1.0);
                            flow[flow_index].idle = (unsigned int) ((flow[flow_index].payload_length+HEADER_LENGTH) // NOTE: round is optional
                                                                    * nb_cycles_per_flit * (tmp_float));
                            break;

                        case VAR_PCK_FXD_IDL:  // It determines the payload length
                            tmp_float = (1.0/flow[flow_index].required_bw - 1.0);
                            flow[flow_index].payload_length = (unsigned int) (flow[flow_index].idle // NOTE: round() is optional
                                                                              / (nb_cycles_per_flit * (tmp_float))) - 1;
                            break;

                        case VAR_PCK_FXD_IAT:  // It determines the payload length and the number of idle cycles
                            flow[flow_index].payload_length = (unsigned int) ((flow[flow_index].iat        // NOTE: round() is optional
                                                                               *flow[flow_index].required_bw)/nb_cycles_per_flit) - 1;
                            flow[flow_index].idle = flow[flow_index].iat - (flow[flow_index].payload_length + 1);
                            break;

                        case VAR_IAT_FXD_PCK:  // It determines the inter-arrival time and the number of idle cycles
                            tmp_float = (1.0/flow[flow_index].required_bw);
                            flow[flow_index].iat = (unsigned int) ((flow[flow_index].payload_length + 1)   // NOTE: round() is optional
                                                                   * nb_cycles_per_flit*tmp_float);
                            flow[flow_index].idle = flow[flow_index].iat - (flow[flow_index].payload_length + 1)*nb_cycles_per_flit;
                            break;

                        case VAR_BST_FXD_IAT:  // It determines the burst size and the size of the last packet
                            // It determines the burst size. If it does not have a decimal part, then
                            // last_payload_lengh = payload_length. If it has a decimal part, then the
                            // burst size is incremented by one packet and the length of this last
                            // packet is determined. Depending on the required bandwidth, the length of
                            // the payload of the last packet can equal 0, 1 or a number <= payload_length
                            tmp_float = ((flow[flow_index].required_bw * flow[flow_index].iat)
                                         / ((float)((flow[flow_index].payload_length+1) * nb_cycles_per_flit)));

                            if (((unsigned int)(100*tmp_float)%100) == 0) { // If decimal part is 0
                                flow[flow_index].burst_size = (unsigned int) tmp_float;
                                flow[flow_index].last_payload_length = flow[flow_index].payload_length;
                            } else {
                                flow[flow_index].burst_size = (unsigned int) (roundf(tmp_float+0.5));
                                tmp_float  = (trunc((fmod( flow[flow_index].required_bw * flow[flow_index].iat,
                                                           (float)(flow[flow_index].payload_length+1) * nb_cycles_per_flit))/ nb_cycles_per_flit));
                                if (tmp_float < 1)
                                    flow[flow_index].last_payload_length = 0;
                                else
                                    if (tmp_float < 2)
                                        flow[flow_index].last_payload_length = 1;
                                    else
                                        flow[flow_index].last_payload_length = ((unsigned int) tmp_float) - 1;
                            }

                            if (flow[flow_index].last_payload_length == 0)
                                flow[flow_index].idle = flow[flow_index].iat - nb_cycles_per_flit
                                        * (((flow[flow_index].burst_size-1)*(flow[flow_index].payload_length + 1)));
                            else
                                flow[flow_index].idle = flow[flow_index].iat
                                        - nb_cycles_per_flit * (((flow[flow_index].burst_size-1)*(flow[flow_index].payload_length + 1))
                                                                   + (flow[flow_index].last_payload_length+1));
                            break;

                        default: break;
                    }
                } while (flow[flow_index].payload_length == 0);
            }
            // end of PARETO-based generation

            //printf("\n%f\t%u\t%u\t%u\t%u",flow[flow_index].required_bw, flow[flow_index].burst_size, flow[flow_index].payload_length, flow[flow_index].last_payload_length, flow[flow_index].idle);

            // printf("\nrequired_bw=%f, payload_length=%u, idle=%u",flow[flow_index].required_bw,flow[flow_index].payload_length,flow[flow_index].idle);


// ZEFERINO
            /////////////////////////////////////////////////////////////////
            // EMULATING PROCESSING (IDLE INTERVAL BEFORE INJECTING A PACKET)
            /////////////////////////////////////////////////////////////////
            // It updates the cycle to send the next packet adding the idle cycles of the
            // current flow to the value previously calculated value. Then, it inserts
            // wait cycles until cycle_to_send_next_pck is reached
            cycle_to_send_next_pck += flow[flow_index].idle;
            while (clock_cycles.read() < cycle_to_send_next_pck)
                wait();

            /////////////////////
            // SENDING THE PACKET
            /////////////////////
            // SENDING PACKETS IN A BURST
            if (flow[flow_index].burst_size != 0) {
                // It sends a burst of packets
                f_send_burst_of_packets(id, cycle_to_send_next_pck, flow[flow_index]);
                snd_wr.write(0);
                snd_data.write(fNull);


                // It increments the packet counters and calculates when the first packet
                // of the next burst of packets have to be injected. But, if last_payload_length
                // equals 0, the last packet is not taken into account because it was not
                // actually sent
                if (flow[flow_index].last_payload_length != 0) {
                    // It increments the packet counters
                    number_of_packets_sent.write(number_of_packets_sent.read() + flow[flow_index].burst_size);
                    flow[flow_index].pck_sent+= flow[flow_index].burst_size;

                    // It calculates when the first packet of the next burst of packets have to be injected
                    cycle_to_send_next_pck += ((flow[flow_index].payload_length+HEADER_LENGTH) * nb_cycles_per_flit * (flow[flow_index].burst_size - 1))
                            + ((flow[flow_index].last_payload_length+HEADER_LENGTH) * nb_cycles_per_flit);
//ZEFERINO                            + flow[flow_index].idle;
                } else {
                    // It increments the packet counters
                    number_of_packets_sent.write(number_of_packets_sent.read() + flow[flow_index].burst_size - 1);
                    flow[flow_index].pck_sent+= flow[flow_index].burst_size - 1;

                    // It calculates when the first packet of the next burst of packets have to be injected
                    cycle_to_send_next_pck += ((flow[flow_index].payload_length+HEADER_LENGTH) * nb_cycles_per_flit * (flow[flow_index].burst_size - 1));
//ZEFERINO                            + flow[flow_index].idle;
                }

            // SENDING PACKETS ONE BY ONE (NOT IN A BURST)
            } else {
                // It sends the packet
                switch (flow[flow_index].switching_type) {
                    case WH : packet_type =  NORMAL;
                        break;

                    case CS : if (flow[flow_index].pck_sent == 0) {
                            // If it is first packet, it alocates the circuit
                            packet_type = ALOC;
                        } else {
                            if (flow[flow_index].pck_sent == (flow[flow_index].pck_2send-1)) {
                                // If it is last packet, it releases the circuit
                                packet_type = RELEASE;
                            } else {
                                // If its not the first and neither the last packets, send it as a normal packet
                                packet_type = NORMAL;
                            }
                        }
                    break;

                default : break;
                }
                f_send_packet(id, cycle_to_send_next_pck, flow[flow_index],flow[flow_index].payload_length, packet_type);

                snd_wr.write(0);
                snd_data.write(fNull);

                // It increments the packet counters
                number_of_packets_sent.write(number_of_packets_sent.read() + 1);
                flow[flow_index].pck_sent++;

                // It calculates when the next packet have to be injected
                cycle_to_send_next_pck += ((flow[flow_index].payload_length+HEADER_LENGTH) * nb_cycles_per_flit);
//ZEFERINO                        + flow[flow_index].idle;
            }

// ZEFERINO
//            // It inserts wait states until cycle_to_inject is reached
//            while(clock_cycles.read() < cycle_to_send_next_pck) wait();

            // It increments the number of total packets sent
            if (flow[flow_index].burst_size != 0)
                pck_counter += flow[flow_index].burst_size;
            else
                pck_counter++;
        }
    }
    eot.write(1);
    wait();

    // It frees the allocated memory for the flows (if there is someone)
    if (nb_of_flows) free(flow);
}


////////////////////////////////////////////////////////////////////////////////
void fg::p_receive()
////////////////////////////////////////////////////////////////////////////////
{
    rcv_rd.write(1);
    number_of_packets_received.write(0);
    wait();

    UIntVar data;
    bool trailer;
    while(1) {
        const Flit f = rcv_data.read();
        data = f.data;
        trailer = data[FLIT_WIDTH-1];
        if ((rcv_rok.read()==1) && trailer) {
            number_of_packets_received.write(number_of_packets_received.read() + 1);
        }

        wait();
    }
}
