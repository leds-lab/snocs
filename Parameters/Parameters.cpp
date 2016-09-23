#include "Parameters.h"

Parameters* Parameters::params = 0; // Defining and initializing

Parameters::Parameters() {
    pm = new PluginManager("simconf.conf");
// Default values
    xSize = 3;
    ySize = 3;
    wordWidth = 34;
    ribWidth = 8;
    fifoInDepth = 4;
    fifoOutDepth = 0; // Without buffer
}

Parameters::Parameters(const Parameters &c) {
    this->pm = c.pm;

    this->xSize = c.xSize;
    this->ySize = c.ySize;
    this->wordWidth = c.wordWidth;
    this->ribWidth = c.ribWidth;
    this->fifoInDepth = c.fifoInDepth;
    this->fifoOutDepth = c.fifoOutDepth;
}

Parameters& Parameters::operator = (const Parameters& c) {

    this->pm = c.pm;

    this->xSize = c.xSize;
    this->ySize = c.ySize;
    this->wordWidth = c.wordWidth;
    this->ribWidth = c.ribWidth;
    this->fifoInDepth = c.fifoInDepth;
    this->fifoOutDepth = c.fifoOutDepth;

    return *this;
}

Parameters* Parameters::instance() {
    if( !params ) {
        params = new Parameters();
    }
    return params;
}

Parameters::~Parameters() {
    if(Parameters::params) {
        delete Parameters::params;
    }
    Parameters::params = 0;
}
