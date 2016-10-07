#include "TrafficMeter.h"

TrafficMeter::TrafficMeter(sc_module_name mn, char *workDir, char *fileName)
    : SoCINModule(mn), workDir(workDir),outFileName(fileName), outFile(NULL),
      i_CLK("TrafficMeter_iCLK"),
      i_RST("TrafficMeter_iRST"),
      i_EOS("TrafficMeter_iEOS"),
      i_CLK_CYCLES("TrafficMeter_iCLK_CYCLES"),
      i_VC_SEL("TrafficMeter_iVC_SEL"),
      i_VALID("TrafficMeter_iVALID"),
      i_RETURN("TrafficMeter_iRETURN"),
      i_DATA("TrafficMeter_iDATA")
{
    unsigned short widthVcSelector = (unsigned short) ceil(log2(NUM_VC));
    this->trafficClassWidth = (unsigned short) ceil(log2(NUMBER_TRAFFIC_CLASSES));
    this->threadIdWidth = (unsigned short) ceil(log2(NUMBER_OF_THREADS));

    if( widthVcSelector > 0 ) {
        i_VC_SEL.init(widthVcSelector);
    }

    SC_CTHREAD(p_PROBE,i_CLK.pos());
    sensitive << i_CLK.pos() << i_RST.pos();

    SC_METHOD(p_FINISH);
    sensitive << i_EOS;
}

TrafficMeter::~TrafficMeter() {}

void TrafficMeter::p_PROBE() {

    char pathFilename[256];
    sprintf(pathFilename,"%s/%s",workDir,outFileName);

    // Open file to write log
    if( (this->outFile = fopen(pathFilename,"wt")) == NULL ) {
        printf("\n[TrafficMeter] ERROR: It is not possible to open file \"%s\" to write log." \
               "\n Verify the write permission on the folder." \
               "\n  Exiting...",pathFilename);
        exit(-1);
    }

    // It prints the header of the table
    fprintf(outFile,"FILE: %s",pathFilename);
    fprintf(outFile,"\n");
#ifdef __WIN32__
    fprintf(outFile,"\n    Packet\tXs\tYs\tXd\tYd\tThread\tTraffic    Deadline\t    Packet\t    Header\t   Trailer\t Packet\t    Req");
    fprintf(outFile,"\n        ID\t  \t  \t  \t  \t    ID\t  Class            \t  Creation\t  at cycle\t  at cycle\t Length\t     BW");
#else
    fprintf(outFile,"\n    Packet\tXs\tYs\tXd\tYd\tThread\tTraffic\t    Deadline\t    Packet\t    Header\t   Trailer\t Packet\t    Req");
    fprintf(outFile,"\n        ID\t  \t  \t  \t  \t    ID\t  Class\t            \t  Creation\t  at cycle\t  at cycle\t Length\t     BW");
#endif

    fprintf(outFile,"\n#\n");

    wait();

    // TODO verificar como implementar para outras técnicas de controle de fluxo
    if( FC_TYPE == 0 ) {    // FC_TYPE == 0 -> Handshake
        while(1) {
            while( i_VALID.read() == 0 ) wait();

            while( i_RETURN.read() == 0) wait();

            this->writeInfo();

            while( i_VALID.read() == 1 ) wait();

            while( i_RETURN.read() == 1 ) wait();
        }
    } else {                // Credit-based
        while(1) {
            if( i_VALID.read() ) {
                this->writeInfo();
            }
            wait();
        }
    }
}


void TrafficMeter::p_FINISH() {

    if( i_EOS.read() == 1 ) {
        fprintf(this->outFile,"\n# %lu", i_CLK_CYCLES.read());
        fclose(outFile);
    }
}

void TrafficMeter::writeInfo() {

    UIntVar v_DATA; // Variable to extract the fieds of the data
    bool    v_BOP;  // Packet framing bit: Begin-of-packet
    bool    v_EOP;  // Packet framing bit: End-of-packet

    Flit dataFlit = i_DATA.read();
    v_DATA = dataFlit.data;

    v_BOP = v_DATA[FLIT_WIDTH-2];
    v_EOP = v_DATA[FLIT_WIDTH-1];

    // Copy the header content and register the cycle of arriving of the header
    if( v_BOP ) {
        this->packetHeader = v_DATA;
        this->cycleOfArriving = i_CLK_CYCLES.read();
    }

    // In the last flit (trailer -> v_EOP) write informations in the log
    if( v_EOP ) {
        Packet* packet = dataFlit.packet_ptr;
        if(packet != NULL) { // For safe packet access
            unsigned short xSrc  = (unsigned short) packetHeader(RIB_WIDTH*2-1, (RIB_WIDTH*2)-(RIB_WIDTH/2) ).to_uint();
            unsigned short ySrc  = (unsigned short) packetHeader((RIB_WIDTH*2)-(RIB_WIDTH/2)-1,RIB_WIDTH).to_uint();
            unsigned short xDest = (unsigned short) packetHeader(RIB_WIDTH-1,RIB_WIDTH/2).to_uint();
            unsigned short yDest = (unsigned short) packetHeader(RIB_WIDTH/2-1,0).to_uint();
            unsigned short msbThreadId = (unsigned short) threadIdWidth != 0 ? THREAD_ID_POSITION + threadIdWidth-1 : THREAD_ID_POSITION;
            unsigned short threadId = (unsigned short) packetHeader(msbThreadId,THREAD_ID_POSITION).to_uint();
            unsigned short msbTrafficClassPos = trafficClassWidth != 0 ? TRAFFIC_CLASS_POSITION+trafficClassWidth-1 : TRAFFIC_CLASS_POSITION;
            unsigned short trafficClass = (unsigned short) packetHeader(msbTrafficClassPos,TRAFFIC_CLASS_POSITION).to_uint();

            fprintf(outFile,"%10lu\t"  , packet->packetId);
            fprintf(outFile,"%2u\t"    , xSrc);
            fprintf(outFile,"%2u\t"    , ySrc);
            fprintf(outFile,"%2u\t"    , xDest);
            fprintf(outFile,"%2u\t"    , yDest);
            fprintf(outFile,"  %2u\t"  , threadId);
            fprintf(outFile,"  %2u\t"  , trafficClass);
            fprintf(outFile,"%10lu\t"  , packet->deadline);
            fprintf(outFile,"%10lu\t"  , packet->packetCreationCycle);
            fprintf(outFile,"%10lu\t"  , this->cycleOfArriving);
            fprintf(outFile,"%10lu\t"  , i_CLK_CYCLES.read());
            fprintf(outFile,"%5u\t"  , packet->payloadLength);
            fprintf(outFile,"  %.2f\t" , round(packet->requiredBW) );
            fprintf(outFile,"\n");
            delete packet;
        } else {
            std::cout << "Flit received without a valid packet reference." << std::endl
                      << "Please verify if in the flow generator on sending flit\n"
                         "if a packet is referenced (pointer assignment)" << std::endl;
        }
    }
}
