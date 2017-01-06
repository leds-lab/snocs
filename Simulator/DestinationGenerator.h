#ifndef __DESTINATIONGENERATOR_H__
#define __DESTINATIONGENERATOR_H__

#include <random>

class DestinationGenerator {
protected:
    // PRNG
    std::default_random_engine generator;
public:
    DestinationGenerator();

    virtual unsigned short getDestination(unsigned short source) = 0;
};

#endif // __DESTINATIONGENERATOR_H__
