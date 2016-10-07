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

// Convert cartesian coordinate of the router in the MESH network to router ID
#define COORDINATE_TO_ID(x,y) (y * X_SIZE + x)

// Forward declaration
class IRouter;

/////////////////////////////////////////////////////////////////////////////////
/// NoC interface without virtual channels
/////////////////////////////////////////////////////////////////////////////////
/*!
 * \brief The INoC class is the interface (abstract class) for concrete
 * implementations of the Network-on-Chip component in the simulator.
 *
 * This interaface represents the SoCINfp structure (w/o virtual channels)
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

    // Routers interface - Local communication ports
    sc_vector<sc_in<Flit> >  i_DATA_IN;
    sc_vector<sc_in<bool> >  i_VALID_IN;
    sc_vector<sc_out<bool> > o_RETURN_IN;
    sc_vector<sc_out<Flit> > o_DATA_OUT;
    sc_vector<sc_out<bool> > o_VALID_OUT;
    sc_vector<sc_in<bool> >  i_RETURN_OUT;

    // Internal units
    std::vector<IRouter *> u_ROUTER;

    INoC(sc_module_name mn,unsigned short nRouters);

    ModuleType moduleType() const { return SoCINModule::TNoC; }

    ~INoC() = 0;
};

inline INoC::~INoC(){}

inline INoC::INoC(sc_module_name mn,unsigned short nRouters)
    : SoCINModule(mn), numRouters(nRouters),
      i_CLK("INoC_iCLK"),
      i_RST("INoC_iRST"),
      i_DATA_IN("INoC_iDATA_IN",nRouters),
      i_VALID_IN("INoC_iVALID_IN",nRouters),
      o_RETURN_IN("INoC_oRETURN_IN",nRouters),
      o_DATA_OUT("INoC_oDATA_OUT",nRouters),
      o_VALID_OUT("INoC_oVALID_OUT",nRouters),
      i_RETURN_OUT("INoC_iRETURN_OUT",nRouters),
      u_ROUTER(nRouters,NULL) {}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// NoC interface with virtual channels //////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
class INoC_VC : public INoC {
protected:
    unsigned short numVirtualChannels;
    unsigned short widthVcSelector;
public:
    // Inherits the common interface from the INoC

    // Virtual channels interface
//    sc_vector<sc_vector<sc_out<bool> > > o_VC;
    sc_vector<sc_vector<sc_in<bool> > >  i_VC_SELECTOR;
    sc_vector<sc_vector<sc_out<bool> > > o_VC_SELECTOR;

    INoC_VC(sc_module_name mn,
            unsigned short nRouters,
            unsigned short nVirtualChannels);

    ~INoC_VC() = 0;
};

inline INoC_VC::~INoC_VC() {}

inline INoC_VC::INoC_VC(sc_module_name mn,
                        unsigned short nRouters,
                        unsigned short nVirtualChannels)
    : INoC(mn,nRouters), numVirtualChannels(nVirtualChannels),
//      o_VC("INoC_VC_oL_VC"),
      i_VC_SELECTOR("INoC_VC_i_VC_SELECTOR",nRouters),
      o_VC_SELECTOR("INoC_VC_o_VC_SELECTOR",nRouters)
{
    widthVcSelector = (unsigned short) ceil(log2(nVirtualChannels));
//    o_VC.init(widthVcSelector);
    for( unsigned short i = 0; i < nRouters; i++ ) {
        i_VC_SELECTOR[i].init(widthVcSelector);
        o_VC_SELECTOR[i].init(widthVcSelector);
    }
}

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
