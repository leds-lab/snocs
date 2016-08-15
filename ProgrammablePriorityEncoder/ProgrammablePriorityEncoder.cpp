#include "ProgrammablePriorityEncoder.h"

ProgrammablePriorityEncoder::ProgrammablePriorityEncoder(
        sc_module_name mn, unsigned short XID, unsigned short YID, unsigned short PORT_ID,
        unsigned short nPorts)
    : sc_module(mn), nPorts(nPorts), i_CLK("PPE_iCLK"), i_RST("PPE_iRST"), i_REQUEST("PPE_iR",nPorts),
      i_PRIORITY("PPE_iP",nPorts), o_GRANT("PPE_oG",nPorts), o_IDLE("PPE_oIDLE"),
      w_IMED_IN("PPE_wIMED_IN",nPorts), w_IMED_OUT("PPE_wIMED_OUT",nPorts), w_GRANT("PPE_wGRANT",nPorts),
      r_GRANT("PPE_rGRANT",nPorts),w_IDLE("PPE_wIDLE"),XID(XID),YID(YID),PORT_ID(PORT_ID)
{

    unsigned short i;
    // Continuar daqui
    SC_METHOD(p_IMED_IN);
    for(i = 0; i < nPorts; i++) {
        sensitive << w_IMED_OUT[i];
    }

    SC_METHOD(p_IMED_OUT);
    for(i = 0; i < nPorts; i++) {
        sensitive << w_IMED_IN[i] << i_REQUEST[i] << i_PRIORITY[i];
    }

    SC_METHOD(p_GRANT);
    for(i = 0; i < nPorts; i++) {
        sensitive << w_IMED_IN[i] << i_REQUEST[i] << i_PRIORITY[i];
    }

    SC_METHOD(p_GRANT_REG);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_IDLE);
    for(i = 0; i < nPorts; i++) {
        sensitive << r_GRANT[i];
    }

    SC_METHOD(p_OUTPUTS);
    sensitive << w_IDLE;
    for(i = 0; i < nPorts; i++) {
        sensitive << r_GRANT[i];
    }

}

////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_IMED_IN()
////////////////////////////////////////////////////////////////////////////////
// Status from the previous arbitration cell
{

    unsigned short i; // Loop iterator
    std::vector<bool> v_IMED_OUT(nPorts); // A copy of w_IMED_OUT for bit-level operations
    std::vector<bool> v_IMED_IN(nPorts);  // A temp. var. used to calculate Imed_in

    for( i = 0; i < nPorts; i++ ) {
        v_IMED_OUT[i] = w_IMED_OUT[i].read();
    }

    v_IMED_IN[0] = v_IMED_OUT[nPorts-1];
    for( i = 1; i < nPorts; i++) {
        v_IMED_IN[i] = v_IMED_OUT[i-1];
    }

    for( i = 0; i < nPorts; i++ ) {
        w_IMED_IN[i].write(v_IMED_IN[i]);
    }
}


////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_GRANT()
////////////////////////////////////////////////////////////////////////////////
// Grant signal sent to the requesting block
{
    unsigned short i; // Loop iterator

    std::vector<bool> v_IMED_IN(nPorts);  // A copy of Imed_in for bit-level operations
    std::vector<bool> v_REQUEST(nPorts);  // A copy of R for bit-level operations
    std::vector<bool> v_PRIORITY(nPorts); // A copy of P for bit-level operations
    std::vector<bool> v_GRANT(nPorts);    // A temp. var. used to calculate Grant

    for( i = 0; i < nPorts; i++ ) {
        v_IMED_IN[i] = w_IMED_IN[i].read();
        v_REQUEST[i] = i_REQUEST[i].read();
        v_PRIORITY[i] = i_PRIORITY[i].read();
    }

    for( i = 0; i < nPorts; i++ ) {
        v_GRANT[i] = v_REQUEST[i] and (not (v_IMED_IN[i] and (not v_PRIORITY[i]) ) );
    }

    for( i = 0; i < nPorts; i++ ) {
        w_GRANT[i].write( v_GRANT[i] );
    }
}


////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_IMED_OUT()
////////////////////////////////////////////////////////////////////////////////
// Status to the next arbitration cell
{
    unsigned short i; // Loop iterator

    std::vector<bool> v_IMED_IN(nPorts);  // A copy of Imed_in for bit-level operations
    std::vector<bool> v_REQUEST(nPorts);  // A copy of R for bit-level operations
    std::vector<bool> v_PRIORITY(nPorts); // A copy of P for bit-level operations
    std::vector<bool> v_IMED_OUT(nPorts); // A temp. var. used to calculate Imed_out

    for( i = 0; i < nPorts; i++ ) {
        v_IMED_IN[i] = w_IMED_IN[i].read();
        v_REQUEST[i] = i_REQUEST[i].read();
        v_PRIORITY[i] = i_PRIORITY[i].read();
    }

    for( i = 0; i < nPorts; i++ ) {
        v_IMED_OUT[i] = v_REQUEST[i] or (v_IMED_IN[i] and (not v_PRIORITY[i]) );
    }

    for( i = 0; i < nPorts; i++ ) {
        w_IMED_OUT[i].write(v_IMED_OUT[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_GRANT_REG()
////////////////////////////////////////////////////////////////////////////////
// Implements the register that stores the grant for a selected request
{
    unsigned short i;

    std::vector<bool> v_GRANT(nPorts);     // A copy of Grant_tmp for bit-level operations
    std::vector<bool> v_REQUEST(nPorts);   // A copy of R for bit-level operations
    std::vector<bool> v_GRANT_REG(nPorts); // A temp. var. used to calculate Grant_reg

    for( i = 0; i < nPorts; i++ ) {
        v_GRANT[i] = w_GRANT[i].read();
        v_REQUEST[i] = i_REQUEST[i].read();
        v_GRANT_REG[i] = r_GRANT[i].read();
    }

    for ( i = 0; i < nPorts; i++ ) {
        if (i_RST.read()) {
            v_GRANT_REG[i] = 0;
        } else { // A register bit can be updated when the arbiter is idle
            if (w_IDLE.read()) {
                v_GRANT_REG[i] = v_GRANT[i];
            } else { // or when a req. is low, specially when a granted request is reset
                if (!v_REQUEST[i]) {
                    v_GRANT_REG[i] = 0;
                }
            }
        }
    }

    for( i = 0; i < nPorts; i++ ) {
        r_GRANT[i].write( v_GRANT_REG[i] );
    }
}


////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_IDLE()
////////////////////////////////////////////////////////////////////////////////
// It signals if the channel scheduled by the arb. is idle (there is no grant)
{
    unsigned short i;

    std::vector<bool> v_GRANT_REG(nPorts); // A copy of Grant_reg for bit-level operations
    bool  v_IDLE;                          // A temp. var. used to calculate s_idle

    for( i = 0; i < nPorts; i++ ) {
        v_GRANT_REG[i] = r_GRANT[i].read();
    }

    v_IDLE = 0;
    for ( i = 0; i < nPorts; i++ ) {
        v_IDLE = v_IDLE or v_GRANT_REG[i];
    }

    w_IDLE.write( not v_IDLE );
}


////////////////////////////////////////////////////////////////////////////////
void ProgrammablePriorityEncoder::p_OUTPUTS()
////////////////////////////////////////////////////////////////////////////////
// It updates the output
{
    o_IDLE.write(w_IDLE);
    for( unsigned short i = 0; i < nPorts; i++) {
        o_GRANT[i].write( r_GRANT[i].read() );
    }
}
