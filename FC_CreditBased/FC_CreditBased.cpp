#include "FC_CreditBased.h"
#include "../export.h"

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief IFC_CreditBased::IFC_CreditBased Constructor that register the processes of
/// Credit-based protocol
/// \param mn Module name
/// \param XID X identifier of router in the network
/// \param YID Y identifier of router in the network
/// \param PORT_ID Port identifier in the router
IFC_CreditBased::IFC_CreditBased(sc_module_name mn,unsigned short XID, unsigned short YID, unsigned short PORT_ID)
    : IInputFlowControl(mn,XID,YID,PORT_ID)
{
    SC_METHOD(p_RETURN);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_VALID);
    sensitive << i_VALID;

#ifdef DEBUG_IFC
    SC_METHOD(p_DEBUG);
    sensitive << i_CLK.pos();
#endif
}

/*!
 * \brief IFC_CreditBased::p_RETURN
 */
void IFC_CreditBased::p_RETURN() {
    if( i_RST.read() == 1 ) {
        o_RETURN.write(0);
    } else {
        o_RETURN.write( i_READ.read() and i_READ_OK.read() );
    }
}

/*!
 * \brief IFC_CreditBased::p_VALID
 */
void IFC_CreditBased::p_VALID() {
    o_WRITE.write( i_VALID.read() );
}

void IFC_CreditBased::p_DEBUG() {

    if( XID == 1 && YID == 0 && PORT_ID == 2) {
        printf("\n[IFC_Credit][%u][%u][%u] ",XID, YID,PORT_ID);
        std::cout  << ", VALID =" << i_VALID.read() \
                   << ", RETURN =" << o_RETURN.read() \
                   << ", WRITE ="  << o_WRITE.read() \
                   << ", READ ="  << i_READ.read();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
/// \brief OFC_CreditBased::OFC_CreditBased Constructor that register the processes of
/// Credit-based protocol
/// \param mn Module name
/// \param numCredits Number of credits of the buffer
/// \param XID X identifier of router in the network
/// \param YID Y identifier of router in the network
/// \param PORT_ID Port identifier in the router
OFC_CreditBased::OFC_CreditBased(sc_module_name mn,
                                 unsigned short numCredits,
                                 unsigned short XID,
                                 unsigned short YID,
                                 unsigned short PORT_ID)
    : IOutputFlowControl(mn,XID,YID,PORT_ID),
      numCredits(numCredits),
      r_COUNTER("OFC_CreditBased_rCOUNTER")
{
    SC_METHOD(p_COUNTER);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_OUTPUTS);
    sensitive << i_READ_OK << r_COUNTER << i_RETURN;

#ifdef DEBUG_OFC
    SC_METHOD(p_DEBUG);
    sensitive << i_CLK.pos();
#endif
}

/*!
 * \brief OFC_CreditBased::p_COUNTER It implements the credit counter.
 * If there is no data to be sent (rok=0) and a credit is received (ret=1),
 * it increments the number of credits.On the other hand, if it is sending
 * a data and no credit is being received, it decrements the number of credits.
 * Otherwise, the number of credits doesn't change.
 */
void OFC_CreditBased::p_COUNTER() {
    if(i_RST.read() == 1) {
        r_COUNTER.write(numCredits);
    } else {
        if( i_READ_OK.read() == 0 ) {
            if( (i_RETURN.read()) && (r_COUNTER.read() != (numCredits)) ) {
                r_COUNTER.write( r_COUNTER.read() + 1 );
            }
        } else {
            if( (i_RETURN.read() == 0) && (r_COUNTER.read() != 0) ) {
                r_COUNTER.write( r_COUNTER.read() - 1 );
            }
        }
    }
}

/*!
 * \brief OFC_CreditBased::p_OUTPUTS It determines the outputs.
 * If there is a flit to be sent (rok=1) and the sender still has
 * at least one credit, the data is sent (val=rd=1). If there
 * is no credit, but the receiver is returning a new credit (ret=1),
 * then, it can also send the data, because there is room in the receiver.
 */
void OFC_CreditBased::p_OUTPUTS() {
    bool p_MOVE; // Auxiliar variable that is set under the conditions for which
                 // a flit can be set
    if( i_READ_OK.read() == 1 ) {
        if( r_COUNTER.read() == 0 ) {
            if( i_RETURN.read() == 1 ) {
                p_MOVE = true;
            } else {
                p_MOVE = false;
            }
        } else {
            p_MOVE = true;
        }
    } else {
        p_MOVE = false;
    }

    o_VALID.write(p_MOVE);
    o_READ.write(p_MOVE);
}

void OFC_CreditBased::p_DEBUG() {
    if( XID == 0 && YID == 0 && PORT_ID == 0) {
        printf("\n[OFC_Credit][%u][%u][%u] ",XID, YID,PORT_ID);
        std::cout  << ", COUNTER=" << r_COUNTER.read() \
                   << ", ROK =" << i_READ_OK.read() \
                   << ", RETURN =" << i_RETURN.read() \
                   << ", VALID =" << o_VALID.read() \
                   << ", READ =" << o_READ.read();
    }
}

///////////////////////////////////////////////////////////////////////////////////////


/*!
 * Factory method for instantation e deallocation of modules from the plugin
 */
extern "C" {
////////////////// IFC Factory //////////////////
    SS_EXP IInputFlowControl* new_IFC(sc_simcontext* simcontext,
                                      sc_module_name moduleName,
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

        return new IFC_CreditBased(moduleName,XID,YID,PORT_ID);
    }
    SS_EXP void delete_IFC(IInputFlowControl* ifc) {
        delete ifc;
    }

////////////////// OFC Factory //////////////////
    SS_EXP IOutputFlowControl* new_OFC(sc_simcontext* simcontext,
                                      sc_module_name moduleName,
                                      unsigned short int XID,
                                      unsigned short int YID,
                                      unsigned short int PORT_ID,
                                      unsigned short int numberOfCredits) {
        // Simcontext is needed because in shared library a
        // new and different simcontext will be created if
        // the main application simcontext is not passed to
        // this shared library.
        // IMPORTANT: The simcontext assignment shall be
        // done before component instantiation.
        sc_curr_simcontext = simcontext;
        sc_default_global_context = simcontext;

        return new OFC_CreditBased(moduleName,numberOfCredits,XID,YID,PORT_ID);
    }
    SS_EXP void delete_OFC(IOutputFlowControl* ofc) {
        delete ofc;
    }
}
