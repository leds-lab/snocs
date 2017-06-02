#include "StopSim.h"
#include "../Parameters/Parameters.h"

//#define DEBUG_STOPSIM

StopSim::StopSim(sc_module_name mn,
                 unsigned short nInterfaces,
                 char *filename)
    : SoCINModule(mn),numInterfaces(nInterfaces),
      i_CLK("StopSim_iCLK"),
      i_RST("StopSim_iRST"),
      o_EOS("StopSim_oEOS"),
      i_CLK_CYCLES("StopSim_iCLK_CYCLES"),
      i_TG_EOT("StopSim_iTG_EOT",nInterfaces),
      i_TG_NUM_PACKETS_SENT("StopSim_iTG_NUM_PACKETS_SENT",nInterfaces),
      i_TG_NUM_PACKETS_RECEIVED("StopSim_iTG_NUM_PACKETS_RECEIVED",nInterfaces),
      r_TOTAL_PACKETS_SENT("StopSim_rTOTAL_PACKETS_SENT"),
      r_TOTAL_PACKETS_RECEIVED("StopSim_rTOTAL_PACKETS_RECEIVED"),
      w_EOT("StopSim_wEOT"),
      confFileName(filename)
{

    this->configureStopOptions();
    SC_CTHREAD(p_STOP,i_CLK.pos());
    sensitive << i_CLK.pos() << i_RST;
}

void StopSim::configureStopOptions() {
    char str[512];
    FILE *fp_in;

    // It opens the input file and reads the stop options
    sprintf(str,"%s/%s.par",WORK_DIR,confFileName);
    if ((fp_in=fopen(str,"r")) == NULL) {
        printf("\n [StopSim] ERROR: Impossible to open file \"%s\".\nExiting...", str);
        exit(1);
    }

    unsigned long int stopTime_ns;
    unsigned long int stopNumPackets;

    fscanf(fp_in,"%s",str);
    stopCycle = atol(str);
    fscanf(fp_in,"%s",str);
    stopTime_ns = atol(str);
    fscanf(fp_in,"%s",str);
    stopNumPackets = atol(str);
    fclose(fp_in);

    if(stopCycle == 0) {
        if( stopTime_ns != 0) {
            stopMethod = ByTime;
            stopCycle = stopTime_ns / CLK_PERIOD;
        } else if(stopNumPackets > 0) {
            totalPacketsToReceive = stopNumPackets;
            stopMethod = ByPacketsDelivered;
        } else {
            stopMethod = AllPacketsDelivered;
        }
    } else {
        stopMethod = ByCycles;
    }

    // It opens the output file
    sprintf(str,"%s/%s.out",WORK_DIR,confFileName);
    if ((fp_out=fopen(str,"wt")) == NULL) {
        printf("\n	[StopSim] ERROR: Impossible to open file \"%s\". Exiting...", str);
        exit(1);
    }
}

void StopSim::p_STOP() {

    // It resets the counters
    r_TOTAL_PACKETS_SENT.write(0);
    r_TOTAL_PACKETS_RECEIVED.write(0);
    o_EOS.write(0);
    wait();

    unsigned long v_NUM_PACKET_SENT,v_NUM_PACKET_RECEIVED;
    bool v_EOT;
    unsigned short i;
    while (1) {
        v_NUM_PACKET_SENT = 0;
        v_NUM_PACKET_RECEIVED = 0;
        v_EOT = true;
        for( i = 0; i < numInterfaces; i++ ) {
            v_NUM_PACKET_SENT += i_TG_NUM_PACKETS_SENT[i].read();
            v_NUM_PACKET_RECEIVED += i_TG_NUM_PACKETS_RECEIVED[i].read();
            v_EOT &= i_TG_EOT[i].read();
        }
        r_TOTAL_PACKETS_SENT.write( v_NUM_PACKET_SENT );
        r_TOTAL_PACKETS_RECEIVED.write( v_NUM_PACKET_RECEIVED );

        w_EOT.write(v_EOT);

#ifdef DEBUG_STOPSIM
        std::cout << std::endl;
        std::cout << "[StopSim] cycle = " << i_CLK_CYCLES.read();

        std::cout << "\n Packets Sent = " << r_TOTAL_PACKETS_SENT.read()
                  << " (";
        for( i = 0; i < numInterfaces; i++ ) {
            std::cout << i_TG_NUM_PACKETS_SENT[i].read() << " + ";
        }
        std::cout << ")"
                  << std::endl;

        std::cout << " Packets Received = " << r_TOTAL_PACKETS_RECEIVED.read()
                  << " (";
        for( i = 0; i < numInterfaces; i++ ) {
            std::cout << i_TG_NUM_PACKETS_RECEIVED[i].read() << " + ";
        }
        std::cout << ")"
                  << std::endl;
#endif // DEBUG_STOPSIM

        switch( stopMethod ) {
            case AllPacketsDelivered:
            case ByPacketsDelivered:
                if( v_NUM_PACKET_RECEIVED >= totalPacketsToReceive ) { // Stop by the number of packets delivered
                    this->endSimulation(fp_out);
                }
                break;
            case ByCycles:
            case ByTime:
                if (i_CLK_CYCLES.read() >= stopCycle) {
                    this->endSimulation(fp_out);
                }
                break;
        }

        wait();
    }
}

void StopSim::endSimulation(FILE* fp_out) {
    fprintf(fp_out,"%llu", i_CLK_CYCLES.read());
    fclose(fp_out);
    o_EOS.write(1);
    wait();
    sc_stop();
}
