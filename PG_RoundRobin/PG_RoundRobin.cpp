#include "PG_RoundRobin.h"

PG_RoundRobin::PG_RoundRobin(sc_module_name mn,unsigned int numReqs_Grants,unsigned short int XID,
                     unsigned short int YID, unsigned short int PORT_ID)
        : IPriorityGenerator(mn,numReqs_Grants,XID,YID,PORT_ID),
          r_UPDATE("PGRR_update_register"), r_GDELAYED("PGRR_Gdelayed",numReqs_Grants),
          r_NEXT_PRIORITIES("PGRR_nextP",numReqs_Grants), r_PRIORITIES("PGRR_Preg",numReqs_Grants)
{

    unsigned short int i;
    SC_METHOD(p_GDELAYED);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_UPDATE);
    for( i = 0; i < numReqs_Grants; i++) {
        sensitive << i_GRANTS[i] << r_GDELAYED[i];
    }

    SC_METHOD(p_NEXT_PRIORITIES);
    for( i = 0; i < numReqs_Grants; i++) {
        sensitive << i_GRANTS[i];
    }

    SC_METHOD(p_PRIORITIES);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_OUTPUTS);
    for( i = 0; i < numReqs_Grants; i++) {
        sensitive << r_PRIORITIES[i];
    }

}

////////////////////////////////////////////////////////////////////////////////
void PG_RoundRobin::p_GDELAYED()
////////////////////////////////////////////////////////////////////////////////
// It's just a d-type register that holds the state of G for one clock cycle
{
    if (i_RST.read())
        for(unsigned short int i = 0; i < numPorts; i++)
            r_GDELAYED[i].write(0);
    else
        for(unsigned short int i = 0; i < numPorts; i++)
            r_GDELAYED[i].write(i_GRANTS[i].read());
}

////////////////////////////////////////////////////////////////////////////////
void PG_RoundRobin::p_UPDATE()
////////////////////////////////////////////////////////////////////////////////
// It just implements a parameterizable OR which detect if any request was
// granted in the last cycle. In this case, it enables the priority register
// to update its state.
{
    unsigned short int i;                   // A variable for loops iteration
    std::vector<bool> v_GRANTING(numPorts); // A request was granted
    std::vector<bool> v_G(numPorts);        // A copy of G for bit-level operations
    std::vector<bool> v_Gdelayed(numPorts); // A copy of Gdelayed for bit-level ops
    bool update_register_tmp;               // A temp. var. used to calculate update_register

    // Reading status
    for(i = 0; i < numPorts; i++) {
        v_G[i] = i_GRANTS[i].read();
        v_Gdelayed[i] = r_GDELAYED[i].read();
    }

    // It first determines if there exists any request that was granted in the
    // last cycle.
    for (i = 0; i < numPorts; i++) {
        v_GRANTING[i] = v_G[i] and (not v_Gdelayed[i]);
    }

    // Then, it makes an OR operation among all the granting(i) bits
    update_register_tmp = 0;
    for (i = 0; i < numPorts; i++) {
        update_register_tmp = update_register_tmp | v_GRANTING[i];
    }

    r_UPDATE.write(update_register_tmp);
}


////////////////////////////////////////////////////////////////////////////////
void PG_RoundRobin::p_NEXT_PRIORITIES()
////////////////////////////////////////////////////////////////////////////////
// It determines the next priority order by rotating 1x left the current
// granting status. Ex. If G="0001", then, nextP="0010". Such rotation will
// ensure that the current granted request (e.g. R(0)) will have the lowest
// priority level at the next arbitration cycle (e.g. P(1)>P(2)> P(3)>P(0)).
{
    unsigned short int i;                  // Variable for loops iteration
    std::vector<bool> v_GRANTS(numPorts);  // A copy of G for bit-level operations
    std::vector<bool> v_NEXT_P(numPorts);  // A temp. var. used to calculate nextP

    // Reading all grants (G(0), G(1), ..., G(n-1))
    for( i = 0; i < numPorts; i++) {
        v_GRANTS[i] = i_GRANTS[i].read();
    }

    // Rotating bits
    v_NEXT_P[0] = v_GRANTS[numPorts-1];
    for ( i = 1; i < numPorts; i++ ) {
        v_NEXT_P[i] = v_GRANTS[i-1];
    }

    // Writing on all outputs (all bits)
    for( i = 0; i < numPorts; i++) {
        r_NEXT_PRIORITIES[i].write(v_NEXT_P[i]);
    }
}


////////////////////////////////////////////////////////////////////////////////
void PG_RoundRobin::p_PRIORITIES()
////////////////////////////////////////////////////////////////////////////////
// It is reset with bit 0 in 1 and the others in 0, and is updated at each
// arbitration cycle (after a request is grant) with the value determined
// for nextP.
{
    unsigned short int i;                 // Variable for loops iteration
    std::vector<bool> v_NEXT_P(numPorts); // A copy of nextP for bit-level operations
    std::vector<bool> v_Preg(numPorts);   // A temp. var. used to calculate Preg

    // Reading status
    for( i = 0; i < numPorts; i++) {
        v_Preg[i] = r_PRIORITIES[i].read();
        v_NEXT_P[i] = r_NEXT_PRIORITIES[i].read();
    }

    if (i_RST.read()) {
        v_Preg[0] = 1;
    } else {
        if (r_UPDATE.read()) {
            v_Preg[0] = v_NEXT_P[0];
        }
    }

    for ( i = 1; i < numPorts; i++) {
        if (i_RST.read()) {
            v_Preg[i] = 0;
        } else {
            if (r_UPDATE.read()) {
                v_Preg[i] = v_NEXT_P[i];
            }
        }
    }

    // Writing on all outputs (all bits)
    for( i = 0; i < numPorts; i++) {
        r_PRIORITIES[i].write(v_Preg[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
void PG_RoundRobin::p_OUTPUTS()
////////////////////////////////////////////////////////////////////////////////
// It updates the output
{
    for(unsigned short int i = 0; i < numPorts; i++) {
        o_PRIORITIES[i].write( r_PRIORITIES[i].read() );
    }
}
