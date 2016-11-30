#include "TrafficMeter.h"
#include "../PluginManager/PluginManager.h"

TrafficMeter::TrafficMeter(sc_module_name mn,
                           char *workDir,
                           char *fileName,
                           INoC::TopologyType topologyType,
                           bool isExternal)
    : SoCINModule(mn),
      pckId(1),// TEMP
      workDir(workDir),outFileName(fileName), outFile(NULL),
      topologyType(topologyType),
      isExternal(isExternal),
      i_CLK("TrafficMeter_iCLK"),
      i_RST("TrafficMeter_iRST"),
      i_EOS("TrafficMeter_iEOS"),
      i_CLK_CYCLES("TrafficMeter_iCLK_CYCLES"),
      i_VC_SEL("TrafficMeter_iVC_SEL"),
      i_VALID("TrafficMeter_iVALID"),
      i_RETURN("TrafficMeter_iRETURN"),
      i_DATA("TrafficMeter_iDATA")
{
    this->initialize();

    unsigned short widthVcSelector = (unsigned short) ceil(log2(NUM_VC));
    this->trafficClassWidth = (unsigned short) ceil(log2(N_CLASSES));

    u_IFC = PLUGIN_MANAGER->inputFlowControlInstance("IFC_TM",0,0);
    u_IFC->i_CLK(i_CLK);
    u_IFC->i_RST(i_RST);
    u_IFC->i_DATA(i_DATA);
    u_IFC->i_VALID(i_VALID);
    u_IFC->i_READ(i_RETURN); // Adapt from read to return
    // Signals unused to traffic meter
    u_IFC->i_READ_OK(w_IFC_READ_OK);
    u_IFC->i_WRITE_OK(w_WRITE_OK);
    u_IFC->o_RETURN(w_RETURN);
    u_IFC->o_WRITE(w_WRITE);

    if( NUM_VC > 1 ) {
        i_VC_SEL.init(widthVcSelector);
    }

    SC_METHOD(p_PROBE);
    sensitive << u_IFC->e_PACKET_RECEIVED;

    SC_METHOD(p_FINISH);
    sensitive << i_EOS;
}

TrafficMeter::~TrafficMeter() {}

void TrafficMeter::initialize() {

    char pathFilename[256];
    sprintf(pathFilename,"%s/%s",workDir,outFileName);

    // Open file to write log
    if( (this->outFile = fopen(pathFilename,"wt")) == NULL ) {
        printf("\n[TrafficMeter] ERROR: It is not possible to open file \"%s\" to write log." \
               "\n Verify if the folder exists and the user write permission is granted." \
               "\n  Exiting...",pathFilename);
        exit(-1);
    }

    // It prints the header of the table
    fprintf(outFile,"FILE: %s",pathFilename);
    fprintf(outFile,"\n");
    #if defined(__WIN32__) || defined(_WIN32)
    //    fprintf(outFile,"\n    Packet\tXs\tYs\tXd\tYd\tThread\tTraffic    Deadline\t    Packet\t    Header\t   Trailer\t Packet\t    Req");
    //    fprintf(outFile,"\n        ID\t  \t  \t  \t  \t    ID\t  Class            \t  Creation\t  at cycle\t  at cycle\t Length\t     BW");
    fprintf(outFile,"\n    Packet\t src\tdest\tThread\tTraffic    Deadline\t    Packet\t    Header\t   Trailer\t Packet\t    Req");
    fprintf(outFile,"\n        ID\t    \t    \t    ID\t  Class            \t  Creation\t  at cycle\t  at cycle\t Length\t     BW");
    #else
    fprintf(outFile,"\n    Packet\tXs\tYs\tXd\tYd\tThread\tTraffic\t    Deadline\t    Packet\t    Header\t   Trailer\t Packet\t    Req");
    fprintf(outFile,"\n        ID\t  \t  \t  \t  \t    ID\t  Class\t            \t  Creation\t  at cycle\t  at cycle\t Length\t     BW");
    #endif

    fprintf(outFile,"\n#\n");
}

void TrafficMeter::p_PROBE() {
    this->writeInfo();
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
            unsigned short src  = this->getPacketSource();
            unsigned short dest = this->getPacketDestination();
            unsigned short trafficClass = (unsigned short) packetHeader(CLS_POS,CLS_POS-2).to_uint();
            unsigned short flowId = (unsigned short) packetHeader(25,24).to_uint();
//            fprintf(outFile,"%10lu\t"  , packet->packetId); // TEMP
            fprintf(outFile,"%10lu\t"  , pckId++);   // TEMP
            fprintf(outFile,"%4u\t"    , src);
            fprintf(outFile,"%4u\t"    , dest);
            fprintf(outFile,"  %2u\t"  , flowId);
            fprintf(outFile,"  %2u\t"  , trafficClass);
            fprintf(outFile,"%10lu\t"  , packet->deadline);
            fprintf(outFile,"%10lu\t"  , packet->packetCreationCycle);
            fprintf(outFile,"%10lu\t"  , this->cycleOfArriving);
            fprintf(outFile,"%10lu\t"  , i_CLK_CYCLES.read());
            fprintf(outFile,"%5u\t"    , packet->payloadLength);
            fprintf(outFile,"  %.2f\t" , round(packet->requiredBW) );
            fprintf(outFile,"\n");
            if(isExternal) {
                delete packet;
                packet = NULL;
            }
        } else {
            std::cout << "Flit received without a valid packet reference." << std::endl
                      << "Please verify if in the flow generator on sending flit\n"
                         "if a packet is referenced (pointer assignment)" << std::endl;
        }
    }
}

unsigned short TrafficMeter::getPacketSource() {
    switch ( topologyType ) {
        case INoC::TT_Non_Orthogonal:
            return packetHeader.range(RIB_WIDTH*2-1,RIB_WIDTH).to_uint();
        case INoC::TT_Orthogonal2D: {
            unsigned xSrc = packetHeader.range(RIB_WIDTH*2-1,RIB_WIDTH*2-RIB_WIDTH/2).to_uint();
            unsigned ySrc = packetHeader.range(RIB_WIDTH*2-RIB_WIDTH/2-1,RIB_WIDTH).to_uint();
            return COORDINATE_2D_TO_ID(xSrc,ySrc);
        }
        case INoC::TT_Orthogonal3D:
            unsigned xSrc = packetHeader.range(15,13).to_uint();
            unsigned ySrc = packetHeader.range(12,10).to_uint();
            unsigned zSrc = packetHeader.range( 9, 8).to_uint();
            return COORDINATE_3D_TO_ID(xSrc,ySrc,zSrc);
    }
    return 0;
}

unsigned short TrafficMeter::getPacketDestination() {
    switch ( topologyType ) {
        case INoC::TT_Non_Orthogonal:
            return packetHeader.range(RIB_WIDTH-1,0).to_uint();
        case INoC::TT_Orthogonal2D: {
            unsigned xDst = packetHeader.range(RIB_WIDTH-1,RIB_WIDTH/2).to_uint();
            unsigned yDst = packetHeader.range(RIB_WIDTH/2-1,0).to_uint();
            return COORDINATE_2D_TO_ID(xDst,yDst);
        }
        case INoC::TT_Orthogonal3D:
            unsigned xDst = packetHeader.range(7,5).to_uint();
            unsigned yDst = packetHeader.range(4,2).to_uint();
            unsigned zDst = packetHeader.range(1,0).to_uint();
            return COORDINATE_3D_TO_ID(xDst,yDst,zDst);
    }
    return 0;
}
