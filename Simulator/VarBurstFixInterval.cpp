#include "VarBurstFixInterval.h"

#include <cmath>

VarBurstFixInterval::VarBurstFixInterval(unsigned short numCyclesPerFlit)
    : TypeInjection(numCyclesPerFlit)
{}

void VarBurstFixInterval::adjustFlow(FlowGenerator::FlowParameters &flow) {

    unsigned int pckSize = (flow.payload_length+HEADER_LENGTH);
    float chr = 1.0f;
    float ipr = flow.required_bw;

    // It determines the burst size. If it does not have a decimal part, then
    // last_payload_lengh = payload_length. If it has a decimal part, then the
    // burst size is incremented by one packet and the length of this last
    // packet is determined. Depending on the required bandwidth, the length of
    // the payload of the last packet can equal 0, 1 or a number <= payload_length

    float bSize = (ipr *flow.iat) / ( chr * pckSize * numCyclesPerFlit); // Equation 12 Leonel Thesis

    if (((unsigned int)(100*bSize)%100) == 0) { // If decimal part is 0
        flow.burst_size = (unsigned int) bSize;
        flow.last_payload_length = flow.payload_length;
    } else {
        flow.burst_size = (unsigned int) (std::roundf(bSize+0.5));
        float lastPckSize = fmod( (ipr * flow.iat), (float)( chr * pckSize * numCyclesPerFlit)); // Equation 13 Leonel thesis
        if (lastPckSize < 1) {
            flow.last_payload_length = 0;
        } else {
            if (lastPckSize < 2) {
                flow.last_payload_length = 1;
            } else {
                flow.last_payload_length = ((unsigned int) lastPckSize) - HEADER_LENGTH;
            }
        }
    }

    if (flow.last_payload_length == 0) {
        flow.idle = flow.iat - numCyclesPerFlit * ((flow.burst_size-1)*(flow.payload_length + HEADER_LENGTH));
    } else {
        flow.idle = flow.iat - numCyclesPerFlit * (((flow.burst_size-1)*(flow.payload_length + HEADER_LENGTH))
                                           + (flow.last_payload_length+HEADER_LENGTH));
    }
}
