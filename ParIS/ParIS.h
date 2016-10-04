/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : ParIS
FILE   : ParIS.h
--------------------------------------------------------------------------------
DESCRIPTION: ParIS router implementation
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 08/09/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
| 03/10/2016 - 1.0     - Eduardo Alves da Silva      | Virtual channels
--------------------------------------------------------------------------------

*/
#ifndef __PARIS_H__
#define __PARIS_H__

#include "../Router/Router.h"

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// ParIS with N virtual channels /////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief The ParIS_N_VC class implements the ParIS router from SoCIN with N virtual
 * channels.
 */
class ParIS_N_VC : public IRouter_VC {
public:

    // Signals - wires - connectors
    sc_vector<sc_vector<sc_vector<sc_signal<bool> > > > w_REQUEST;  // Requests[VirtualChannel][IN][OUT]
    sc_vector<sc_vector<sc_vector<sc_signal<bool> > > > w_GRANT;    // Grants[VirtualChannel][OUT][IN]
    sc_vector<sc_vector<sc_signal<bool> > >             w_READ_OK;  // Read status[VirtualChannel][IN]
    sc_vector<sc_vector<sc_signal<bool> > >             w_READ;     // Read commands[VirtualChannel][OUT]
    sc_vector<sc_vector<sc_signal<bool> > >             w_IDLE;     // Idle status[VirtualChannel][OUT]
    sc_vector<sc_vector<sc_signal<Flit> > >             w_DATA;     // Data buses[VirtualChannel][IN]

    sc_signal<bool> w_GND;

    // Internal Units - a router is composed by Input and Output modules
    std::vector<XIN_N_VC*>  u_XIN;
    std::vector<XOUT_N_VC*> u_XOUT;

    // Module's process
    void p_GND() {
        w_GND.write(0);
    }

    sc_trace_file* tf;

    void p_DEBUG();

    SC_HAS_PROCESS(ParIS_N_VC);
    ParIS_N_VC(sc_module_name mn,
               unsigned short nPorts,
               unsigned short nVirtualChannels,
               unsigned short XID,
               unsigned short YID);

    const char* moduleName() const { return "ParIS_N_VC"; }

    ~ParIS_N_VC();
};


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// ParIS from SoCINfp //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief The ParIS class implements the ParIS router from SoCIN
 */
class ParIS : public IRouter {
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

    // Module's process
    void p_GND() {
        w_GND.write(0);
    }

    sc_trace_file* tf;

    void p_DEBUG();

    SC_HAS_PROCESS(ParIS);
    ParIS(sc_module_name mn,
          unsigned short nPorts,
          unsigned short XID,
          unsigned short YID);

    const char* moduleName() const { return "ParIS"; }

    ~ParIS();
};

#endif // __PARIS_H__
