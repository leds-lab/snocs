/*
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : SoCINModule - Global module used by all others components
EXTRAS : Data type used in communication channels (Flit) and a bit width
        (word length) type used to accomodate the real data
FILE   : SoCINGlobal.h
--------------------------------------------------------------------------------
DESCRIPTION: The global interface for modules in the SoCIN simulator
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 25/08/2016 - 1.0     - Eduardo Alves da Silva      | Initial implementation
--------------------------------------------------------------------------------
*/

//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Guidelines ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///
/// (1) Use header identification - as used in top of this file
/// template:
/*!
--------------------------------------------------------------------------------
PROJECT: SoCIN_Simulator
MODULE : <Module_Name>
FILE   : <Filename>.h
--------------------------------------------------------------------------------
DESCRIPTION: <General information about the modules in the file>
--------------------------------------------------------------------------------
AUTHORS: <Research group | member group>
CONTACT: <Main associated in the project email: e.g.: e-mail@institution.com>
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
|[dd/MM/YYYY]- M.m     - <Last update responsable>   | <Brief information>
--------------------------------------------------------------------------------
*/
///
/// (2) Use the initially style of coding adopted in this project
/// This style is based on original VHDL guidelines of the LEDS
/// with some adjusts
///
/// Rules:
/// 1 - ) Each signal and variable must be declared one per line and preferably
/// must have included a comment on the final of the line that identifies its
/// functionality for the component (help in the understand)
/// 2 - ) The tabs used in the identation of the code must be of 2 or 4 spaces (preferably 4)
/// 3 - ) Each process could define (writes in signals and ports) only one signal
/// value(RT approach)
/// 4 - ) Reserved words of the C++ must be written in lowercase (class, int, bool,...)
/// 5 - ) Specific identifiers of the model must be written with all letters in UPPERCASE,
/// except prefix and sufix (e.g. i_CLK, o_DATA, ...)
/// 6 - ) Signals enabled in low level (or falling edge) must be identified with the
/// sufix _n (e.g. i_RST_n)
/// 7 - ) All identifiers must be preceded by one prefix, as follow: (according its class)
///     * i_: Input signal (in mode)
///     * o_: Output signal (out and buffer mode)
///     * b_: Bidirectional signal (inout mode)
///     * w_: Internal signal that implements a wire
///     * r_: Internal signal that implements a register
///     * v_: Variable of process
///     * p_: Process identifier
///     * u_: Identifier of process or component instance (a unit)
///     * t_: Identifier of data type (typedef) - don't use in functions typedef that are used to instantiate plugins
///     * c_: Identifier of constant
///     * s_: Identifier of a state of a state machine
///
/// (3) Please, add doxygen comments in the code always as possible
///
/// (4) All interface classes must be identified by "I" (e.g. IRouter) in its start name,
/// except the global interface of the system (SoCINModule)
///
/// (5) Write testbenchs for all units
///
/// (6) Maintain your code organized and follow the best practices in C++ development
///
/// !!! THANKS !!!
//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Guidelines ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifndef SOCINGLOBAL_H
#define SOCINGLOBAL_H

// Detect Windows OS
#ifdef _WIN32
#  ifdef _SHARED
#    define SS_EXP __declspec(dllexport)
#  else
#    define SS_EXP __declspec(dllimport)
#  endif
// Unix-based - Linux and OS X
#elif __unix__ || __APPLE__
#    define SS_EXP __attribute__((visibility("default")))
#    define SS_IMP __attribute__((visibility("default")))
#else
#    error "Unknown_compiler"
#endif

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

///////////////////////////////////////////////////////////
/// Global definitions
///////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
/// SoCIN Global Module to be used in all components interfaces and
/// implementation
/////////////////////////////////////////////////////////////////////////
/*!
 * \brief The SoCINModule class is the interface for all
 * SoCIN modules
 */
class SoCINModule : public sc_module {
public:

    /*!
     * \brief The ModuleType enum Determines the type of module implemented
     */
    enum ModuleType { Router = 1,
                      Routing,
                      FlowControl,
                      InputFlowControl,
                      OutputFlowControl,
                      Arbiter,
                      Memory,
                      PriorityGenerator,
                      PriorityEncoder,
                      NetworkInterface,
                      Switch,
                      Other
                    };

    virtual ModuleType moduleType() const = 0;
    virtual const char* moduleName() const = 0;

    SoCINModule(sc_module_name mn) : sc_module (mn) {}

    virtual ~SoCINModule() = 0;
};
inline SoCINModule::~SoCINModule() {}
/////////////////////////////////////////////////////////////////////////
/// END of SoCIN Global Module
/////////////////////////////////////////////////////////////////////////



#endif // SOCINGLOBAL_H
