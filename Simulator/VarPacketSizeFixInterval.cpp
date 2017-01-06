#include "VarPacketSizeFixInterval.h"

VarPacketSizeFixInterval::VarPacketSizeFixInterval(unsigned short numCyclesPerFlit)
    : TypeInjection(numCyclesPerFlit)
{}

void VarPacketSizeFixInterval::adjustFlow(FlowGenerator::FlowParameters &flow) {
    float chr = 1.0f;
    float ipr = flow.required_bw;

    flow.payload_length = (unsigned int) (flow.iat * ipr) / ( numCyclesPerFlit * chr) - HEADER_LENGTH; // Equation 9 Leonel Thesis
    flow.idle = flow.iat - (flow.payload_length + HEADER_LENGTH);
}
