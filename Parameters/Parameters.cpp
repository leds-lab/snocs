#include "Parameters.h"
#include "../PluginManager/PluginManager.h"

Parameters* Parameters::params = 0; // Defining and initializing

Parameters::Parameters() {
    pm = new PluginManager();
// Default values
    clkPeriod = 1;
    traceSystem = true;
    workDir = const_cast<char*>("work");
    pluginsDir = const_cast<char*>("./plugins");
    confFile = const_cast<char*>("simconf.conf");

    xSize = 3;
    ySize = 3;
    wordWidth = 34;
    ribWidth = 8;
    fifoInDepth = 4;
    fifoOutDepth = 0; // Without buffer
}

Parameters::Parameters(const Parameters &c) {
    this->pm = c.pm;

    this->clkPeriod = c.clkPeriod;
    this->traceSystem = c.traceSystem;
    this->workDir = c.workDir;
    this->pluginsDir = c.pluginsDir;
    this->confFile = c.confFile;

    this->xSize = c.xSize;
    this->ySize = c.ySize;
    this->wordWidth = c.wordWidth;
    this->ribWidth = c.ribWidth;
    this->fifoInDepth = c.fifoInDepth;
    this->fifoOutDepth = c.fifoOutDepth;
}

Parameters& Parameters::operator = (const Parameters& c) {
    this->pm = c.pm;

    this->clkPeriod = c.clkPeriod;
    this->traceSystem = c.traceSystem;
    this->workDir = c.workDir;
    this->pluginsDir = c.pluginsDir;
    this->confFile = c.confFile;

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
