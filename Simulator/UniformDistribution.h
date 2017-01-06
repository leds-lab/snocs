#ifndef __UNIFORMDISTRIBUTION_H__
#define __UNIFORMDISTRIBUTION_H__

#include "DestinationGenerator.h"

class UniformDistribution : public DestinationGenerator {
private:
    std::uniform_int_distribution<int> distribution;
public:
    UniformDistribution();

    unsigned short getDestination(unsigned short source);
};

#endif // UNIFORMDISTRIBUTION_H
