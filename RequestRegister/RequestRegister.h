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

#include "../SoCINModule.h"
#include "../SoCINDefines.h"

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
    ModuleType moduleType() const { return SoCINModule::ReqRegister; }
    const char* moduleName() const { return "RequestRegister"; }
    ~RequestRegister() {}
};

///////////////////////////////////////////////////////////////////////////////////////////////
//// Implementation
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
/// \brief RequestRegister::RequestRegister module constructor
/// \param nPorts Number of ports - requests
/// \param XID X identifier of router in the network
/// \param YID Y identifier of router in the network
/// \param PORT_ID Port identifier in the router
inline RequestRegister::RequestRegister(sc_module_name mn,
                                 unsigned short nPorts,
                                 unsigned short XID,
                                 unsigned short YID,
                                 unsigned short PORT_ID)
    : SoCINModule(mn),
      numPorts(nPorts),
      i_CLK("ReqReg_iCLK"),
      i_RST("ReqReg_iRST"),
      i_READ_OK("ReqReg_iREAD_OK"),
      i_READ("ReqReg_iREAD"),
      i_DATA("ReqReg_iDATA"),
      i_REQUEST("ReqReg_iREQUEST",nPorts),
      o_REQUEST("ReqReg_oREQUEST",nPorts),
      o_REQUESTING("ReqReg_oREQUESTING"),
      r_REQUEST("ReqReg_rREQUEST",nPorts),
      w_BOP("ReqReg_wBOP"),
      w_CIRCUIT_ALLOCATE("ReqReg_wCIRCUIT_ALLOCATE"),
      w_CIRCUIT_RELEASE("ReqReg_wCIRCUIT_RELEASE"),
      r_CIRCUIT_SET("ReqReg_rCIRCUIT_SET"),
      w_TRAILER_SENT("ReqReg_wTRAILER"),
      XID(XID), YID(YID), PORT_ID(PORT_ID)
{
    // Registering processes
    SC_METHOD(p_INTERNAL_SIGNALS);
    sensitive << i_DATA << i_READ_OK << i_READ << r_CIRCUIT_SET << o_REQUESTING;

    SC_METHOD(p_REQUESTS_REGISTERS);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_CIRCUIT_SET);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_OUTPUTS);
    for(unsigned short i = 0; i < nPorts; i++) {
        sensitive << r_REQUEST[i];
    }
}

/*!
 * \brief RequestRegister::p_INTERNAL_SIGNALS Determines the internal signals
 * of allocation both to packet switching and circuit switching.
 *
 * Writes in signals:
 * w_BOP
 * w_CIRCUIT_ALLOCATE
 * w_CIRCUIT_RELEASE
 * w_TRAILER_SENT
 */
inline void RequestRegister::p_INTERNAL_SIGNALS() {
    UIntVar    v_DATA;         // Used to extract fields from din
    sc_uint<2> v_CS_COMMAND;   // Circuit-switching command
    bool       v_BOP;          // Begin-of-packet marker
    bool       v_EOP;          // End-of-packet marker
    bool       v_ALLOCATE;     // Circuit-switching command is CS_ALLOCATE
    bool       v_RELEASE;      // Circuit-switching command is CS_RELEASE
    bool       v_CS_ALLOCATE;  // Allocate circuit
    bool       v_CS_RELEASE;   // Release  circuit
    bool       v_TRAILER_SENT; // Packet trailer was sent

    // It copies the data input to extract the framing and circuit switching commands/bits
    Flit f = i_DATA.read();
    v_DATA = f.data;

    // It decodes the framing commands (bits)
    v_BOP = v_DATA[FLIT_WIDTH-2];   //(not v_data[FLIT_TYPE_WIDTH-1]) and v_data[FLIT_TYPE_WIDTH-2];
    v_EOP = v_DATA[FLIT_WIDTH-1];   //v_data[FLIT_TYPE_WIDTH-1]  and (not v_data[FLIT_TYPE_WIDTH-2]);

    // It decodes the CS command (allocate and release)
    v_CS_COMMAND[1] = v_DATA[RIB_WIDTH*2+1];
    v_CS_COMMAND[0] = v_DATA[RIB_WIDTH*2];
    v_ALLOCATE      = (not v_CS_COMMAND[1]) and      v_CS_COMMAND[0];
    v_RELEASE       = (    v_CS_COMMAND[1]) and  not v_CS_COMMAND[0];

    // It checks if there is a header and if the header includes a circuit Allocate or Release command
    v_CS_ALLOCATE = i_READ_OK.read() and i_READ.read() and v_BOP and v_ALLOCATE;
    v_CS_RELEASE  = i_READ_OK.read() and i_READ.read() and v_BOP and v_RELEASE;

    // It determines if a trailer of a packet was delivered (it is used to cancel the current connection)
    v_TRAILER_SENT = i_READ_OK.read() and i_READ.read() and v_EOP and (not r_CIRCUIT_SET.read());

    // It copies the variable to signals to send them to the other process
    w_BOP.write(v_BOP);
    w_CIRCUIT_ALLOCATE.write(v_CS_ALLOCATE);
    w_CIRCUIT_RELEASE.write(v_CS_RELEASE);
    w_TRAILER_SENT.write(v_TRAILER_SENT);
}

/*!
 * \brief RequestRegister::p_REQUESTS_REGISTERS This process implements a
 * register which determines the state of the requests to the output channels.
 */
inline void RequestRegister::p_REQUESTS_REGISTERS() {
    unsigned short i;
    if (i_RST.read()==1) {
        for(i = 0; i < numPorts; i++) {
            r_REQUEST[i].write(0);
        }
    } else {
        // If there is no registered request and a header is present,
        // it registers the new request determined by the routing function
        if ((i_READ_OK.read()==1) && (w_BOP.read()==1) && (o_REQUESTING.read()==0)) {
// TODO: Verificar como ficará o mapeamento caso o sistema de identificação de portas mude
            for(i = 0; i < numPorts; i++) {
                if( PORT_ID == i ) {
                    r_REQUEST[i].write(0);
                } else {
                    r_REQUEST[i].write( i_REQUEST[i].read() );
                }
            }
      /*
            // REQUEST LOCAL (reqL is not registered by module L)
            if (PORT_ID==LOCAL_ID)
                reqL.write(0);
            else
                reqL.write(in_reqL.read());

            // REQUEST NORTH (reqN is not registered by module N)
            if (PORT_ID==NORTH_ID)
                reqN.write(0);
            else
                reqN.write(in_reqN.read());

            // REQUEST EAST (reqE is not registered by module E
            if (PORT_ID==EAST_ID)
                reqE.write(0);
            else
                reqE.write(in_reqE.read());

            // REQUEST SOUTH (reqS is not registered by module S)
            if (PORT_ID==SOUTH_ID)
                reqS.write(0);
            else
                reqS.write(in_reqS.read());

            // REQUEST WEST (reqW is not registered by module W)
            if (PORT_ID==WEST_ID)
                reqW.write(0);
            else
                reqW.write(in_reqW.read());
     */
        } else {
            if (w_TRAILER_SENT.read()){
                for( i = 0; i < numPorts; i++) {
                    r_REQUEST[i].write(0);
                }
            }
        }
    }
}

/*!
 * \brief RequestRegister::p_CIRCUIT_SET Register that
 * Allocate, desallocate or hold the circuit according
 * of circuit status signals
 */
inline void RequestRegister::p_CIRCUIT_SET() {
    if (i_RST.read() == 1) {
        r_CIRCUIT_SET.write(0);
    } else {
        // TODO: Verificar se esta implementação é mais adequada
        /*
        if( w_CIRCUIT_ALLOCATE.read() == 1 ) {
            r_CIRCUIT_SET.write(1);
        } else if( w_CIRCUIT_RELEASE.read() == 1 ) {
            r_CIRCUIT_SET.write(0);
        } else {
            r_CIRCUIT_SET.write( r_CIRCUIT_SET.read() );
        } */

        if ((w_CIRCUIT_ALLOCATE.read()==1) || (w_CIRCUIT_RELEASE.read()==1)) {
            if (w_CIRCUIT_ALLOCATE.read()==1) {
                // If there is a CS_ALLOCATE command, it sets   the circuit
                r_CIRCUIT_SET.write(1);
            } else {
                // If there is a CS_RELEASE  command, it resets the circuit
                r_CIRCUIT_SET.write(0);
            }
        } else {
            // In the other cases, it holds the current state
            r_CIRCUIT_SET.write(r_CIRCUIT_SET.read());
        }
    }
}

/*!
 * \brief RequestRegister::p_OUTPUTS Updates the outputs
 */
inline void RequestRegister::p_OUTPUTS() {
    bool v_REQ;                 // Temporary variable to read request signals
    bool v_REQUESTING = false;  // Temporary variable to OR operation between all requests
    for( unsigned short i = 0; i < numPorts; i++ ) {
        v_REQ = r_REQUEST[i].read();
        v_REQUESTING = v_REQUESTING | v_REQ;
        o_REQUEST[i].write( v_REQ );
    }
    o_REQUESTING.write(v_REQUESTING);
}



#endif // REQUESTREGISTER_H
