#include "VarIntervalFixPacketSize.h"

VarIntervalFixPacketSize::VarIntervalFixPacketSize(unsigned short numCyclesPerFlit)
    : TypeInjection(numCyclesPerFlit)
{}

void VarIntervalFixPacketSize::adjustFlow(FlowGenerator::FlowParameters &flow) {

    unsigned int pckSize = (flow.payload_length+HEADER_LENGTH);
    float chr = 1.0f;
    float ipr = flow.required_bw;

    flow.iat = (unsigned int) pckSize * numCyclesPerFlit * (chr / ipr);   // Equation 10 Leonel Thesis
    flow.idle = flow.iat - pckSize * numCyclesPerFlit;
}
