/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : INoC
FILE   : NoC.h
--------------------------------------------------------------------------------
DESCRIPTION: Network interface (abstract class) for NoC implementations
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 08/09/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef __NOC_H__
#define __NOC_H__

#include "../SoCINModule.h"
#include "../SoCINDefines.h"

/////////////////////////////////////////////////////////////////////////////////
/// NoC interface
/////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief The INoC class is the interface (abstract class) for concrete
 * implementations of the Network-on-Chip component in the simulator.
 *
 * This interface contais the port communication in a vector style i.e.
 * use sc_vector.
 * The ports are:
 * * Input interface
 *   - i_DATA_IN[]   :-> Input data channels to put data in the network.
 *   - i_VALID_IN[]  :-> Input signal for flow control to put data in the network.
 *   - o_RETURN_IN[] :-> Output signal for flow control on putting data in the network.
 * * Output interface
 *   - o_DATA_OUT[]  :-> Output data channels to extract data of the network.
 *   - o_VALID_OUT[] :-> Output signals for flow control to extract data of the network.
 *   - i_RETURN_OUT[]:-> Input signal for flow control on extracting data of the network.
 *
 * Each index in the vector represents the signals of a router.
 */
class INoC : public SoCINModule {
protected:
    unsigned short numRouters;
public:
    // INTERFACE
    // System signals
    sc_in<bool> i_CLK;
    sc_in<bool> i_RST;

    // Routers interface - Communication ports
    sc_vector<sc_in<Flit> >  i_DATA_IN;
    sc_vector<sc_in<bool> >  i_VALID_IN;
    sc_vector<sc_out<bool> > o_RETURN_IN;
    sc_vector<sc_out<Flit> > o_DATA_OUT;
    sc_vector<sc_out<bool> > o_VALID_OUT;
    sc_vector<sc_in<bool> >  i_RETURN_OUT;

    // Internal units
    std::vector<IRouter *> u_ROUTER;

    INoC(sc_module_name mn);

    ModuleType moduleType() const { return SoCINModule::Network; }

    ~INoC() = 0;
};

inline INoC::~INoC(){}

inline INoC::INoC(sc_module_name mn)
    : SoCINModule(mn),
      i_CLK("INoC_iCLK"),
      i_RST("INoC_iRST"),
      i_DATA_IN("INoC_iDATA_IN"),
      i_VALID_IN("INoC_iVALID_IN"),
      o_RETURN_IN("INoC_oRETURN_IN"),
      o_DATA_OUT("INoC_oDATA_OUT"),
      o_VALID_OUT("INoC_oVALID_OUT"),
      i_RETURN_OUT("INoC_iRETURN_OUT") {}


/////////////////////////////////////////////////////////////
/// Typedefs for Factory Methods of concrete NoCs
/////////////////////////////////////////////////////////////
/*!
 * \brief create_NoC Typedef for instantiate a NoC
 * \param sc_simcontext A pointer of simulation context (required for correct plugins use)
 * \param sc_module_name Name for the module to be instantiated
 * \return A method for instantiate a NoC
 */
typedef INoC* create_NoC(sc_simcontext*,sc_module_name);

/*!
 * \brief destroy_NoC Typedef for deallocating a NoC
 */
typedef void destroy_NoC(INoC*);
/////////////////////////////////////////////////////////////


#endif // __NOC_H__
