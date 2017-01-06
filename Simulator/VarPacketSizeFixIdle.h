#ifndef __VARPACKETSIZEFIXIDLE_H__
#define __VARPACKETSIZEFIXIDLE_H__

#include "TypeInjection.h"

class VarPacketSizeFixIdle : public TypeInjection {
public:
    VarPacketSizeFixIdle(unsigned short numCyclesPerFlit);

    void adjustFlow(FlowGenerator::FlowParameters& flow);
};

#endif // __VARPACKETSIZEFIXIDLE_H__
