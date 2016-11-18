#include "Parameters.h"
#include "../PluginManager/PluginManager.h"

Parameters* Parameters::params = 0; // Defining and initializing

Parameters::Parameters() {
    pckId = 1;

    pm = new PluginManager();
// Default values
    clkPeriod = 1;
    traceSystem = false;
    workDir = const_cast<char*>("work");
    pluginsDir = const_cast<char*>("./plugins");
    confFile = const_cast<char*>("simconf.conf");

    xSize = 4;
    ySize = 4;
    zSize = 4;

    wordWidth = 34;
    ribWidth = 8;
    trafficClassPosition = 18;
    numberOfClasses = 8;
    threadIdPosition = 30;
    numberOfThreads = 4;

    numVirtualChannels = 0;
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
    this->zSize = c.zSize;
    this->wordWidth = c.wordWidth;
    this->ribWidth = c.ribWidth;
    this->numVirtualChannels = c.numVirtualChannels;
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
    this->zSize = c.zSize;
    this->wordWidth = c.wordWidth;
    this->ribWidth = c.ribWidth;
    this->numVirtualChannels = c.numVirtualChannels;
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

