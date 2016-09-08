/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : IFlowControl - IInputFlowControl - IOutputFlowControl
FILE   : FlowControl.h
--------------------------------------------------------------------------------
DESCRIPTION: They are the interfaces classes that implement flow controllers
which determines the resources allocation in the network by implementing a
specific protocol.
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 24/08/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from ParIS
--------------------------------------------------------------------------------
*/
#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include "../SoCINModule.h"

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Interface for Flow Controllers (IFC and OFC)
/////////////////////////////////////////////////////////////
/*!
 * \brief The IFlowControl class is an interface
 * (abstract class) for Input Flow Controller and Output
 * Flow Controller
 */
class IFlowControl : public SoCINModule {
public:
    // Interface - System
    sc_in_clk       i_CLK;      // Clock
    sc_in<bool>     i_RST;      // Reset

    unsigned short int XID, YID, PORT_ID;

    IFlowControl(sc_module_name mn,
                 unsigned short XID,
                 unsigned short YID,
                 unsigned short PORT_ID)
        : SoCINModule(mn),
          i_CLK("FC_iCLK"),
          i_RST("FC_iRST"),
          XID(XID),
          YID(YID),
          PORT_ID(PORT_ID) {}

    ~IFlowControl() = 0;
};
inline IFlowControl::~IFlowControl(){}
/////////////////////////////////////////////////////////////
/// END Interface for Flow Controllers
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Interface for Input Flow Controllers (IFCs)
/////////////////////////////////////////////////////////////
/*!
 * \brief The IInputFlowControl class is an interface
 * (abstract class) for Input Flow Control
 */
class IInputFlowControl : public IFlowControl {
public:
    // Link Interface
    sc_in<bool>  i_VALID;   // Data validation
    sc_out<bool> o_RETURN;  // Return (credit or acknowledgement)

    // FIFO Interface
    sc_out<bool> o_WRITE;   // Command to write a data into the FIFO
    sc_in<bool>  i_WRITE_OK;// FIFO has room to be written (not full)
    sc_in<bool>  i_READ;    // Command to read a data from the FIFO
    sc_in<bool>  i_READ_OK; // FIFO has a data to be read (not empty)
    sc_in<bool>  i_DATA;    // FIFO data output

    IInputFlowControl(sc_module_name mn,
                       unsigned short XID,
                       unsigned short YID,
                       unsigned short PORT_ID)
              : IFlowControl(mn,XID,YID,PORT_ID),
                i_VALID("IFC_iVALID"),
                o_RETURN("IFC_oRETURN"),
                o_WRITE("IFC_oWRITE"),
                i_WRITE_OK("IFC_iWRITE_OK"),
                i_READ("IFC_iREAD"),
                i_READ_OK("IFC_iREAD_OK"),
                i_DATA("IFC_iDATA") {}

    ~IInputFlowControl() = 0;
};
inline IInputFlowControl::~IInputFlowControl() {}
/////////////////////////////////////////////////////////////
/// END Interface for Input Flow Controllers (IFCs)
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Interface for Output Flow Controllers (OFCs)
/////////////////////////////////////////////////////////////
/*!
* \brief The IOutputFlowControl class is an interface
* (abstract class) for Output Flow Control
*/

class IOutputFlowControl : public IFlowControl {
public:
    // Link Interface
    sc_out<bool> o_VALID;   // Data validation
    sc_in<bool>  i_RETURN;  // Return (credit or acknowledgement)

    // FIFO Interface
    sc_out<bool> o_READ;    // Command to read a data from the FIFO
    sc_in<bool>  i_READ_OK; // FIFO has a data to be read (not empty)

    IOutputFlowControl(sc_module_name mn,
                       unsigned short XID,
                       unsigned short YID,
                       unsigned short PORT_ID)
              : IFlowControl(mn,XID,YID,PORT_ID),
                o_VALID("OFC_oVALID"),
                i_RETURN("OFC_iRETURN"),
                o_READ("OFC_oREAD"),
                i_READ_OK("OFC_iREAD_OK") {}

    ~IOutputFlowControl() = 0;

};
inline IOutputFlowControl::~IOutputFlowControl() {}
/////////////////////////////////////////////////////////////
/// END Interface for Output Flow Controllers (OFCs)
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Typedefs for Factory Methods of concrete IFCs
/////////////////////////////////////////////////////////////
/*!
 * \brief create_InputFlowControl Typedef for instantiate a
 * Input Flow Controller
 * \param sc_simcontext A pointer of simulation context (required for correct plugins use)
 * \param sc_module_name Name for the module to be instantiated
 * \param XID Column identifier of the router in the network
 * \param YID Row identifier of the router in the network
 * \param PORT_ID Port identifier of the router
 * \return A method for instantiate a Input Flow Controller
 */
typedef IInputFlowControl* create_InputFlowControl(sc_simcontext*,
                                                   sc_module_name,
                                                   unsigned short int XID,
                                                   unsigned short int YID,
                                                   unsigned short int PORT_ID);

/*!
 * \brief destroy_InputFlowControl Typedef for deallocating a
 * Input Flow Controller
 */
typedef void destroy_InputFlowControl(IInputFlowControl*);
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// Typedefs for Factory Methods of concrete OFCs
/////////////////////////////////////////////////////////////
/*!
 * \brief create_OuputFlowControl Typedef for instantiate a
 * Output Flow Controller with buffers depth
 * \param sc_simcontext A pointer of simulation context (required for correct plugins use)
 * \param sc_module_name Name for the module to be instantiated
 * \param XID Column identifier of the router in the network
 * \param YID Row identifier of the router in the network
 * \param PORT_ID Port identifier of the router
 * \param bufferDepth Buffers depth - used in credit-based flow control (number of credits)
 * \return A method for instantiate a Output Flow Controller
 */
typedef IOutputFlowControl* create_OutputFlowControl(sc_simcontext*,
                                                     sc_module_name,
                                                     unsigned short int XID,
                                                     unsigned short int YID,
                                                     unsigned short int PORT_ID,
                                                     unsigned short int bufferDepth);

/*!
 * \brief destroy_OutputFlowControl Typedef for deallocating a
 * Output Flow Controller
 */
typedef void destroy_OutputFlowControl(IOutputFlowControl*);
/////////////////////////////////////////////////////////////

// TODO: To implement testbenchs for Input and Output Flow Controllers

/////////////////////////////////////////////////////////////
///  Testbench Input Flow Control
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
///  Testbench Output Flow Control
/////////////////////////////////////////////////////////////

#endif // FLOWCONTROL_H
