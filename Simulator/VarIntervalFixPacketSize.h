#ifndef __VARINTERVALFIXPACKETSIZE_H__
#define __VARINTERVALFIXPACKETSIZE_H__

#include "TypeInjection.h"

class VarIntervalFixPacketSize : public TypeInjection {
public:
    VarIntervalFixPacketSize(unsigned short numCyclesPerFlit);

    void adjustFlow(FlowGenerator::FlowParameters& flow);
};

#endif // VARINTERVALFIXPACKETSIZE_H
