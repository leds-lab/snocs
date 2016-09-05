/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : RequestRegister
FILE   : RequestRegister.h
--------------------------------------------------------------------------------
DESCRIPTION: Register responsible to hold a request after the routing function
schedules an output channel to be used by an incoming packet. A registered
request is hold until the packet trailer is delivered. It includes a circuitry
to set a circuit in order to reduce the latency for a communication flow
(the headers' hop latency is reduced from 3 cycles to 1 cycle)
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 10/08/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef REQUESTREGISTER_H
#define REQUESTREGISTER_H

#include "../GlobalDefs.h"

/*!
 * \brief The RequestRegister class implement the registers that
 * hold the requests until the trailer be received.
 *
 * Register responsible to hold a request after the routing function
 * schedules an output channel to be used by an incoming packet. A registered
 * request is hold until the packet trailer is delivered. It includes a circuitry
 * to set a circuit in order to reduce the latency for a communication flow
 * (the headers' hop latency is reduced from 3 cycles to 1 cycle)
 */
class RequestRegister : public SoCINModule {
protected:
    unsigned short numPorts; // Number of requests
public:
    // System signals
    sc_in_clk   i_CLK;       // Clock
    sc_in<bool> i_RST;       // Reset

    // FIFO interface
    sc_in<bool> i_READ_OK;   // FIFO has a data to be read (not empty)
    sc_in<bool> i_READ;      // Command to read a data from the FIFO
    sc_in<Flit> i_DATA;      // FIFO data output

    // Register interface
    sc_vector<sc_in<bool> >  i_REQUEST;     // Input Requests
    sc_vector<sc_out<bool> > o_REQUEST;     // Output Requests
    sc_out<bool>             o_REQUESTING;  // There exists someone requesting

    // Internal signals
    sc_vector<sc_signal<bool> > r_REQUEST;           // Register requests
    sc_signal<bool>             w_BOP;               // Begin-of-Packet framing bit
    sc_signal<bool>             w_CIRCUIT_ALLOCATE;  // Command to allocate a circuit
    sc_signal<bool>             w_CIRCUIT_RELEASE;   // Command to release a circuit connection
    sc_signal<bool>             r_CIRCUIT_SET;       // Register status of circuit is set
    sc_signal<bool>             w_TRAILER_SENT;      // Trailer was sent

    // Internal data structures
    unsigned short int XID, YID, PORT_ID;

    // Module's processes
    void p_REQUESTS_REGISTERS();
    void p_INTERNAL_SIGNALS();
    void p_CIRCUIT_SET();
    void p_OUTPUTS();

    SC_HAS_PROCESS(RequestRegister);
    RequestRegister(sc_module_name mn,
                    unsigned short nPorts,
                    unsigned short XID,
                    unsigned short YID,
                    unsigned short PORT_ID);
};

#endif // REQUESTREGISTER_H
