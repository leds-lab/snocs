#include "Parameters.h"

Parameters* Parameters::params = 0; // Defining and initializing

Parameters::Parameters() {
// Default values
    wordWidth = 34;
    ribWidth = 8;
    xSize = 3;
    ySize = 3;
}

Parameters::Parameters(const Parameters &c) {
    this->wordWidth = c.wordWidth;
    this->ribWidth = c.ribWidth;
    this->xSize = c.xSize;
    this->ySize = c.ySize;
}

Parameters& Parameters::operator = (const Parameters& c) {
    this->wordWidth = c.wordWidth;
    this->ribWidth = c.ribWidth;
    this->xSize = c.xSize;
    this->ySize = c.ySize;
    return *this;
}

Parameters* Parameters::instance() {
    if( !params ) {
        params = new Parameters();
    }
    return params;
}

Parameters::~Parameters() {
    Parameters::params = 0;
}
