#include "ConstantInjection.h"

ConstantInjection::ConstantInjection(unsigned short numCyclesPerFlit)
    : TypeInjection(numCyclesPerFlit)
{}

void ConstantInjection::adjustFlow(FlowGenerator::FlowParameters &){} // Nothing to do, the front-end calculates the idle time
