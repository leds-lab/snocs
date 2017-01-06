#ifndef __VARIABLEIDLEFIXPACKETSIZE_H__
#define __VARIABLEIDLEFIXPACKETSIZE_H__

#include "TypeInjection.h"

class VariableIdleFixPacketSize : public TypeInjection {
public:
    VariableIdleFixPacketSize(unsigned short numCyclesPerFlit);

    void adjustFlow(FlowGenerator::FlowParameters& flow);
};

#endif // VARIABLEIDLEFIXPACKETSIZE_H
