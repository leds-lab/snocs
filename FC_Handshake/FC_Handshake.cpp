#include "FC_Handshake.h"

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief IFC_Handshake::IFC_Handshake Constructor that register the processes of handshake
/// protocol
/// \param mn Module name
/// \param XID X identifier of router in the network
/// \param YID Y identifier of router in the network
/// \param PORT_ID Port identifier in the router
IFC_Handshake::IFC_Handshake(sc_module_name mn,
                             unsigned short XID,
                             unsigned short YID,
                             unsigned short PORT_ID)
    : IInputFlowControl(mn,XID,YID,PORT_ID),
      r_CUR_STATE("IFC_Handshake_rCUR_STATE"),
      w_NEXT_STATE("IFC_Handshake_wNEXT_STATE")
{
    SC_METHOD(p_CURRENT_STATE);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_NEXT_STATE);
    sensitive << r_CUR_STATE << i_VALID << i_WRITE_OK;

    SC_METHOD(p_OUTPUTS);
    sensitive << r_CUR_STATE;
}

/*!
 * \brief IFC_Handshake::p_CURRENT_STATE This process implements the state register of the FSM
 */
void IFC_Handshake::p_CURRENT_STATE() {
    if( i_RST.read() == 1 ) {
        r_CUR_STATE.write(s_S0);
    } else {
        r_CUR_STATE.write(w_NEXT_STATE.read());
    }
}

/*!
 * \brief IFC_Handshake::p_NEXT_STATE This process implements combinational circuit of the next
 * state function of the FSM
 */
void IFC_Handshake::p_NEXT_STATE() {
    switch(r_CUR_STATE.read()) {
    // Waits a new incoming data (val=1) and, if the FIFO is not full
    // (wok=1), goes to the S1 state in order to receive the data.
        case s_S0: {
            if( (i_VALID.read()) && (i_WRITE_OK.read()) ){
                w_NEXT_STATE.write(s_S1);
            } else {
                w_NEXT_STATE.write(s_S0);
            }
            break;
        }
    // Writes the data into the FIFO and goes back to the S0 state
    // if val=0, or, if not, goes to S2 state.
        case s_S1: {
            if( !i_VALID.read() ) {
                w_NEXT_STATE.write(s_S0);
            } else {
                w_NEXT_STATE.write(s_S2);
            }
            break;
        }
    // Waits val goes to 0 to complete the data transfer.
        case s_S2: {
            if( !i_VALID.read() ) {
                w_NEXT_STATE.write(s_S0);
            } else {
                w_NEXT_STATE.write(s_S2);
            }
            break;
        }
        default: {
            w_NEXT_STATE.write(s_S0);
        }
    }
}

/*!
 * \brief IFC_Handshake::p_OUTPUTS This process updates the outputs
 */
void IFC_Handshake::p_OUTPUTS() {
    switch(r_CUR_STATE.read()) {
    // Do nothing
        case s_S0: {
            o_RETURN.write(0);
            o_WRITE.write(0);
            break;
        }
    // Acknowledges the data and writes it into the FIFO
        case s_S1: {
            o_RETURN.write(1);
            o_WRITE.write(1);
            break;
        }
    // Holds the acknowledge high while valid is not low
        case s_S2: {
            o_RETURN.write(1);
            o_WRITE.write(0);
            break;
        }
        default: {
            o_RETURN.write(0);
            o_WRITE.write(0);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief OFC_Handshake::OFC_Handshake Constructor that register the processes of handshake
/// protocol
/// \param mn Module name
/// \param XID X identifier of router in the network
/// \param YID Y identifier of router in the network
/// \param PORT_ID Port identifier in the router
OFC_Handshake::OFC_Handshake(sc_module_name mn,unsigned short XID,unsigned short YID, unsigned short PORT_ID)
    : IOutputFlowControl(mn,XID,YID,PORT_ID),
      r_CUR_STATE("OFC_Handshake_rCUR_STATE"),
      w_NEXT_STATE("OFC_Handshake_wNEXT_STATE")
{
    SC_METHOD(p_CURRENT_STATE);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_NEXT_STATE);
    sensitive << r_CUR_STATE << i_RETURN << i_READ_OK;

    SC_METHOD(p_OUTPUTS);
    sensitive << r_CUR_STATE;
}

/*!
 * \brief OFC_Handshake::p_CURRENT_STATE This process implements the state register of the FSM
 */
void OFC_Handshake::p_CURRENT_STATE() {
    if( i_RST.read() == 1 ) {
        r_CUR_STATE.write(s_S0);
    } else {
        r_CUR_STATE.write(w_NEXT_STATE.read());
    }
}

/*!
 * \brief OFC_Handshake::p_NEXT_STATE This process implements combinational circuit of the next
 * state function of the FSM
 */
void OFC_Handshake::p_NEXT_STATE() {
    switch(r_CUR_STATE.read()) {
    // If there is a data to be sent in the selected input channel (rok=1)
    // and the receiver is not busy (ret=0), goes to the S1 state in order
    // to send the data.
        case s_S0: {
            if( (!i_RETURN.read()) && (i_READ_OK.read()) ){
                w_NEXT_STATE.write(s_S1);
            } else {
                w_NEXT_STATE.write(s_S0);
            }
            break;
        }
    // It sends the data and, when the data is received (ret=1), goes to the
    // state S2 in order to notify the sender that the data was delivered.
        case s_S1: {
            if( i_RETURN.read() ) {
                w_NEXT_STATE.write(s_S2);
            } else {
                w_NEXT_STATE.write(s_S1);
            }
            break;
        }
    // It notifies the sender that the data was delivered and returns to state
    // S0 or S1 (under the same conditions used in S0).
        case s_S2: {
            if( (!i_RETURN.read()) && (i_READ_OK.read()) ) {
                w_NEXT_STATE.write(s_S1);
            } else {
                w_NEXT_STATE.write(s_S0);
            }
            break;
        }
        default: {
            w_NEXT_STATE.write(s_S0);
        }
    }
}

/*!
 * \brief OFC_Handshake::p_OUTPUTS This process updates the outputs
 */
void OFC_Handshake::p_OUTPUTS() {
    switch(r_CUR_STATE.read()) {
    // Do nothing
        case s_S0: {
            o_VALID.write(0);
            o_READ.write(0);
            break;
        }
    // Validates the outgoing data.
        case s_S1: {
            o_VALID.write(1);
            o_READ.write(0);
            break;
        }
    // Notifies the sender that the data was sent.
        case s_S2: {
            o_VALID.write(0);
            o_READ.write(1);
            break;
        }
        default: {
            o_VALID.write(0);
            o_READ.write(0);
        }
    }
}
