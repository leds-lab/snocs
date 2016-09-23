/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : IRouter
FILE   : Router.h
--------------------------------------------------------------------------------
DESCRIPTION: Router interface (abstract class)
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 08/09/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/

#ifndef __ROUTER_H__
#define __ROUTER_H__

#include "../XIN/XIN.h"
#include "../XOUT/XOUT.h"

/////////////////////////////////////////////////////////////////////////////////
/// Router interface
/////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief The IRouter class is an interface (abstract class) for router
 * implementations.
 *
 * This interface is based on Paris router.
 */
class IRouter : public SoCINModule {
protected:
    unsigned short numPorts;
public:
    // Interface
    // System signals
    sc_in<bool> i_CLK;  // Clock
    sc_in<bool> i_RST;  // Reset

    // Ports
    sc_vector<sc_in<Flit> >  i_DATA_IN;     // Data of input channels
    sc_vector<sc_in<bool> >  i_VALID_IN;    // Valid of input channels
    sc_vector<sc_out<bool> > o_RETURN_IN;   // Return of input channels
    sc_vector<sc_out<Flit> > o_DATA_OUT;    // Data of output channels
    sc_vector<sc_out<bool> > o_VALID_OUT;   // Valid of output channels
    sc_vector<sc_in<bool> >  i_RETURN_OUT;  // Return of output channels

    // Internal Units - a router is composed by Input and Output modules
    std::vector<XIN*>  u_XIN;
    std::vector<XOUT*> u_XOUT;
//    sc_vector<XIN>  u_XIN;
//    sc_vector<XOUT> u_XOUT;

    // Internal data structures
    unsigned short XID,YID;

    IRouter(sc_module_name mn,
            unsigned short nPorts,
            unsigned short XID,
            unsigned short YID);

    ModuleType moduleType() const { return SoCINModule::Router; }
    ~IRouter() = 0;
};

inline IRouter::~IRouter(){}

inline IRouter::IRouter(sc_module_name mn,
                        unsigned short nPorts,
                        unsigned short XID,
                        unsigned short YID)
    : SoCINModule(mn),numPorts(nPorts),
      i_CLK("IRouter_iCLK"),
      i_RST("IRouter_iRST"),
      i_DATA_IN("IRouter_iDATA_IN",nPorts),
      i_VALID_IN("IRouter_iVALID_IN",nPorts),
      o_RETURN_IN("IRouter_oRETURN_IN",nPorts),
      o_DATA_OUT("IRouter_oDATA_OUT",nPorts),
      o_VALID_OUT("IRouter_oVALID_OUT",nPorts),
      i_RETURN_OUT("IRouter_iRETURN_OUT",nPorts),
      u_XIN(nPorts,NULL),
      u_XOUT(nPorts,NULL),
      XID(XID),YID(YID) {}

/////////////////////////////////////////////////////////////
/// Typedefs for Factory Methods of concrete Routers
/////////////////////////////////////////////////////////////
/*!
 * \brief create_Router Typedef for instantiate a Router
 * \param sc_simcontext A pointer of simulation context (required for correct plugins use)
 * \param sc_module_name Name for the module to be instantiated
 * \param nPorts Number of ports of the Router
 * \param XID Column identifier of the router in the network
 * \param YID Row identifier of the router in the network
 * \return A method for instantiate a Router
 */
typedef IRouter* create_Router(sc_simcontext*,
                               sc_module_name,
                               unsigned short int nPorts,
                               unsigned short int XID,
                               unsigned short int YID);

/*!
 * \brief destroy_Router Typedef for deallocating a Router
 */
typedef void destroy_Router(IRouter*);
/////////////////////////////////////////////////////////////

#endif // __ROUTER_H__
