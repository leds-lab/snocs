#include "VarIdleFixPacketSize.h"

VariableIdleFixPacketSize::VariableIdleFixPacketSize(unsigned short numCyclesPerFlit)
    : TypeInjection(numCyclesPerFlit)
{}

void VariableIdleFixPacketSize::adjustFlow(FlowGenerator::FlowParameters &flow) {
    unsigned int pckSize = (flow.payload_length+HEADER_LENGTH);
    float chr = 1.0f;
    float ipr = flow.required_bw;

    flow.idle = pckSize * numCyclesPerFlit * ( chr/ipr - 1 ); // Equation 6 Leonel Thesis
}
