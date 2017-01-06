#ifndef __VARBURSTFIXINTERVAL_H__
#define __VARBURSTFIXINTERVAL_H__

#include "TypeInjection.h"

class VarBurstFixInterval : public TypeInjection {
public:
    VarBurstFixInterval(unsigned short numCyclesPerFlit);

    void adjustFlow(FlowGenerator::FlowParameters& flow);
};

#endif // VARBURSTFIXINTERVAL_H
