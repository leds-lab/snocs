#include "DestinationGenerator.h"

#include "../Parameters/Parameters.h"
#include <chrono>

DestinationGenerator::DestinationGenerator()
    : generator(std::chrono::system_clock::now().time_since_epoch().count())
{}
