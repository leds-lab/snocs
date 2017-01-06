#ifndef __VARPACKETSIZEFIXINTERARRIVAL_H__
#define __VARPACKETSIZEFIXINTERARRIVAL_H__

#include "TypeInjection.h"

class VarPacketSizeFixInterval : public TypeInjection {
public:
    VarPacketSizeFixInterval(unsigned short numCyclesPerFlit);

    void adjustFlow(FlowGenerator::FlowParameters& flow);
};

#endif // VARPACKETSIZEFIXINTERARRIVAL_H
