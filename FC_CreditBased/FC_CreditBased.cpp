#include "FC_CreditBased.h"


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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
/// \brief OFC_CreditBased::OFC_CreditBased Constructor that register the processes of
/// Credit-based protocol
/// \param mn Module name
/// \param numCredits Number of credits of the buffer
/// \param XID X identifier of router in the network
/// \param YID Y identifier of router in the network
/// \param PORT_ID Port identifier in the router
OFC_CreditBased::OFC_CreditBased(sc_module_name mn, unsigned short numCredits, unsigned short XID, unsigned short YID, unsigned short PORT_ID)
    : IOutputFlowControl(mn,XID,YID,PORT_ID),
      numCredits(numCredits),
      r_COUNTER("OFC_CreditBased_rCOUNTER")
{
    SC_METHOD(p_COUNTER);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_OUTPUTS);
    sensitive << i_READ_OK << r_COUNTER << i_RETURN;
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