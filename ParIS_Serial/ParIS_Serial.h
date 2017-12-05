/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : ParIS_Serial
FILE   : ParIS_Serial.h
--------------------------------------------------------------------------------
DESCRIPTION: ParIS router implementation with serialized links
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 05/12/2017 - 0.1     - Douglas Rossi de Melo       | Serial links
|                      - Eduardo Alves da Silva      | first implementation
--------------------------------------------------------------------------------

*/
#ifndef __PARIS_SERIAL_H__
#define __PARIS_SERIAL_H__

#include "../src/Router.h"
#include "Converter.h"

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// ParIS with N virtual channels /////////////////////////////
//////////////////////////////// and "serial" links ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief The ParIS_N_VC_Serial class implements the ParIS router from SoCIN with N virtual
 * channels and serialized link.
 */
class ParIS_N_VC_Serial : public IRouter_VC {
public:

    // Signals - wires - connectors
    sc_vector<sc_vector<sc_vector<sc_signal<bool> > > > w_REQUEST;  // Requests[VirtualChannel][IN][OUT]
    sc_vector<sc_vector<sc_vector<sc_signal<bool> > > > w_GRANT;    // Grants[VirtualChannel][OUT][IN]
    sc_vector<sc_vector<sc_signal<bool> > >             w_READ_OK;  // Read status[VirtualChannel][IN]
    sc_vector<sc_vector<sc_signal<bool> > >             w_READ;     // Read commands[VirtualChannel][OUT]
    sc_vector<sc_vector<sc_signal<bool> > >             w_IDLE;     // Idle status[VirtualChannel][OUT]
    sc_vector<sc_vector<sc_signal<Flit> > >             w_DATA;     // Data buses[VirtualChannel][IN]

    sc_signal<bool> w_GND;

    // Internal Units - a router is composed by Input, Output and converter (parallel-serial-parallel) modules
    std::vector<XIN_N_VC*>  u_XIN;
    std::vector<XOUT_N_VC*> u_XOUT;

    // Converter
    std::vector<Converter*> u_CONVERTER; // UNITS - according with number of ports
    sc_vector<sc_signal<bool> > w_VALID; // VALID wires
    sc_vector<sc_signal<bool> > w_RETURN;// RETURN wires
    sc_vector<sc_signal<Flit> > w_DATA_CONV;  // DATA wires to converter

    // Module's process
    void p_GND() {
        w_GND.write(0);
    }

    sc_trace_file* tf;

    void p_DEBUG();

    SC_HAS_PROCESS(ParIS_N_VC_Serial);
    ParIS_N_VC_Serial(sc_module_name mn,
               unsigned short nPorts,
               unsigned short nVirtualChannels,
               unsigned short ROUTER_ID);

    const char* moduleName() const { return "ParIS_N_VC_Serial"; }

    ~ParIS_N_VC_Serial();
};


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// ParIS from SoCINfp //////////////////////////////////
///////////////////////////////// and "serial" links //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief The ParIS_Serial class implements the ParIS router from SoCIN with serialized link
 */
class ParIS_Serial : public IRouter {
public:

    // Signals - wires - connectors
    sc_vector<sc_vector<sc_signal<bool> > > w_REQUEST;  // Requests
    sc_vector<sc_vector<sc_signal<bool> > > w_GRANT;    // Grants
    sc_vector<sc_signal<bool> >             w_READ_OK;  // Read status
    sc_vector<sc_signal<bool> >             w_READ;     // Read commands
    sc_vector<sc_signal<bool> >             w_IDLE;     // Idle status
    sc_vector<sc_signal<Flit> >             w_DATA;     // Data buses

    sc_signal<bool>                         w_GND;

    // Internal Units - a router is composed by Input and Output modules
    std::vector<XIN_none_VC*>  u_XIN;
    std::vector<XOUT_none_VC*> u_XOUT;

    // Converter
    std::vector<Converter*> u_CONVERTER; // UNITS - according with number of ports
    sc_vector<sc_signal<bool> > w_VALID; // VALID wires
    sc_vector<sc_signal<bool> > w_RETURN;// RETURN wires
    sc_vector<sc_signal<Flit> > w_DATA_CONV;  // DATA wires to converter

    // Module's process
    void p_GND() {
        w_GND.write(0);
    }

    sc_trace_file* tf;

    void p_DEBUG();

    SC_HAS_PROCESS(ParIS_Serial);
    ParIS_Serial(sc_module_name mn,
          unsigned short nPorts,
          unsigned short ROUTER_ID);

    const char* moduleName() const { return "ParIS_Serial"; }

    ~ParIS_Serial();
};

#endif // __PARIS_SERIAL_H__
