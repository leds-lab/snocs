#ifndef __CONSTANTINJECTION_H__
#define __CONSTANTINJECTION_H__

#include "TypeInjection.h"

class ConstantInjection : public TypeInjection {
public:
    ConstantInjection(unsigned short numCyclesPerFlit);
    void adjustFlow(FlowGenerator::FlowParameters& flow);
};

#endif // CONSTANTINJECTION_H
