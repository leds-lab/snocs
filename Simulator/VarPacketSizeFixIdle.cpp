#include "VarPacketSizeFixIdle.h"

VarPacketSizeFixIdle::VarPacketSizeFixIdle(unsigned short numCyclesPerFlit)
    : TypeInjection(numCyclesPerFlit)
{}

void VarPacketSizeFixIdle::adjustFlow(FlowGenerator::FlowParameters &flow) {

    float chr = 1.0f;
    float ipr = flow.required_bw;

    flow.payload_length = (unsigned int) flow.idle / ( (chr/ipr-1) * numCyclesPerFlit ) - HEADER_LENGTH; // Equation 7 Leonel Thesis
}
