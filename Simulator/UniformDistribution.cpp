#include "UniformDistribution.h"

#include "../Parameters/Parameters.h"

UniformDistribution::UniformDistribution()
    : DestinationGenerator(), distribution(0,NUM_ELEMENTS-1)
{}

unsigned short UniformDistribution::getDestination(unsigned short source) {

    unsigned short dest;
    do {
        dest = distribution(generator);
    } while(dest == source);
    return dest;
}
