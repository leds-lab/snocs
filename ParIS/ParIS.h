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
*/
#ifndef PARIS_H
#define PARIS_H

#include "../Router/Router.h"

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

    // Module's process
    void p_GND() {
        w_GND.write(0);
    }

    SC_HAS_PROCESS(ParIS);
    ParIS(sc_module_name mn,
          unsigned short nPorts,
          unsigned short XID,
          unsigned short YID);

    const char* moduleName() const { return "R_Paris"; }
};

#endif // PARIS_H
