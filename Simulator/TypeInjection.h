#ifndef __TYPEINJECTION_H__
#define __TYPEINJECTION_H__

#include "FlowGenerator.h"

class TypeInjection {
protected:
    unsigned short numCyclesPerFlit;
public:
    TypeInjection(unsigned short numCyclesPerFlit);

    virtual void adjustFlow(FlowGenerator::FlowParameters& flow) = 0;
};

#endif // __TYPEINJECTION_H__
