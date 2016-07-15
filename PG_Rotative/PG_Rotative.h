#ifndef PG_ROTATIVE_H
#define PG_ROTATIVE_H

#include "../PriorityGenerator/PriorityGenerator.h"
/*!
 * \brief The PG_Rotative class
 */
class PG_Rotative : public PriorityGenerator {
public:

    // Internal signals
    sc_signal<bool>             update_register; //! Command to up to date Preg
    sc_vector<sc_signal<bool> > Gdelayed;        //! G delayed in 1 cycle
    sc_vector<sc_signal<bool> > nextP;           //! Next priorities values
    sc_vector<sc_signal<bool> > Preg;            //! Priorities register

    // Module's processes
    void p_gdelayed();
    void p_update_register();
    void p_nextp();
    void p_preg();
    void p_outputs();
    void p_debug();

    const char* moduleTypeName() { return "PG_Rotative"; }

    SC_HAS_PROCESS(PG_Rotative);
    /*!
     * \brief PG_Rotative
     * \param mn
     * \param numReqs_Grants
     * \param XID
     * \param YID
     * \param PORT_ID
     */
    PG_Rotative(sc_module_name mn,
              unsigned short int numReqs_Grants,
              unsigned short int XID,
              unsigned short int YID,
              unsigned short int PORT_ID);

};

extern "C" {
    SS_EXP PriorityGenerator* new_PG(sc_simcontext* simcontext,
                              sc_module_name moduleName,
                              unsigned short int numReqs_Grants,
                              unsigned short int XID,
                              unsigned short int YID,
                              unsigned short int PORT_ID) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new PG_Rotative(moduleName,numReqs_Grants,XID,YID,PORT_ID);
    }
    SS_EXP void delete_PG(PriorityGenerator* pg) {
        delete pg;
    }
}


#endif // PG_ROTATIVE_H
