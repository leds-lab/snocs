#include "StopSim.h"
#include "../Parameters/Parameters.h"

//#define DEBUG_STOPSIM

StopSim::StopSim(sc_module_name mn,
                 unsigned short nRouters,
                 char *filename)
    : SoCINModule(mn),numRouters(nRouters),
      i_CLK("StopSim_iCLK"),
      i_RST("StopSim_iRST"),
      o_EOS("StopSim_oEOS"),
      i_CLK_CYCLES("StopSim_iCLK_CYCLES"),
      i_TG_EOT("StopSim_iTG_EOT",nRouters),
      i_TG_NUM_PACKETS_SENT("StopSim_iTG_NUM_PACKETS_SENT",nRouters),
      i_TG_NUM_PACKETS_RECEIVED("StopSim_iTG_NUM_PACKETS_RECEIVED",nRouters),
      r_TOTAL_PACKETS_SENT("StopSim_rTOTAL_PACKETS_SENT"),
      r_TOTAL_PACKETS_RECEIVED("StopSim_rTOTAL_PACKETS_RECEIVED"),
      w_EOT("StopSim_wEOT"),
      confFileName(filename)
{
    SC_CTHREAD(p_STOP,i_CLK.pos());
    sensitive << i_CLK.pos() << i_RST;
}

void StopSim::p_STOP() {

    unsigned long long int stopTime_ns;
    unsigned long long int stopTime_cycles;
    char str[512];
    FILE *fp_in;
    FILE *fp_out;

    // It opens the input file and reads the stop time
    sprintf(str,"%s.par",confFileName);
    if ((fp_in=fopen(str,"r")) == NULL) {
        printf("\n [StopSim] ERROR: Impossible to open file \"%s\".\nExiting...", str);
        exit(1);
    }
    fscanf(fp_in,"%s",str);
    stopTime_cycles = atol(str);
    fscanf(fp_in,"%s",str);
    stopTime_ns = atol(str);
    fclose(fp_in);

    if(stopTime_cycles == 0 && stopTime_ns != 0) {
        stopTime_cycles = stopTime_ns / CLK_PERIOD;
    }

    // It opens the output file
    sprintf(str,"%s.out",confFileName);
    if ((fp_out=fopen(str,"wt")) == NULL) {
        printf("\n	[StopSim] ERROR: Impossible to open file \"%s\". Exiting...", str);
        exit(1);
    }

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
        for( i = 0; i < numRouters; i++ ) {
            v_NUM_PACKET_SENT += i_TG_NUM_PACKETS_SENT[i].read();
            v_NUM_PACKET_RECEIVED += i_TG_NUM_PACKETS_RECEIVED[i].read();
            v_EOT &= i_TG_EOT[i].read();
        }
        r_TOTAL_PACKETS_SENT.write( v_NUM_PACKET_SENT );
        r_TOTAL_PACKETS_RECEIVED.write( v_NUM_PACKET_RECEIVED );

        w_EOT.write(v_EOT);

#ifdef DEBUG_STOPSIM
        cout << "\n";
        cout << "[StopSim] cycle = " << i_CLK_CYCLES.read();

        std::cout << "\n Packets Sent = " << r_TOTAL_PACKETS_SENT.read() << " (";
        for( i = 0; i < numRouters; i++ ) {
            std::cout << i_TG_NUM_PACKETS_SENT[i].read() << " + ";
        }
        std::cout << ")" << std::endl;

        std::cout << "\n Packets Received = " << r_TOTAL_PACKETS_RECEIVED.read() << " (";
        for( i = 0; i < numRouters; i++ ) {
            std::cout << i_TG_NUM_PACKETS_RECEIVED[i].read() << " + ";
        }
        std::cout << ")" << std::endl;
#endif // DEBUG_STOPSIM

//        wait();
        if (stopTime_cycles == 0) {
            if (w_EOT.read() == 1) {
                if (r_TOTAL_PACKETS_SENT.read()==r_TOTAL_PACKETS_RECEIVED.read()) {
                    fprintf(fp_out,"%lu", i_CLK_CYCLES.read());
                    fclose(fp_out);
                    o_EOS.write(1);
                    wait();
                    sc_stop();
                }
            }
        } else {
            if (i_CLK_CYCLES.read() >= stopTime_cycles) {
                fprintf(fp_out,"%lu", i_CLK_CYCLES.read());
                fclose(fp_out);
                o_EOS.write(1);
                wait();
                sc_stop();
            }
        }
        wait();
    }
}
