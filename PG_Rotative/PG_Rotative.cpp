#include "PG_Rotative.h"
#include "../export.h"

PG_Rotative::PG_Rotative(sc_module_name mn,
                     unsigned short numReqs_Grants,
                     unsigned short int XID,
                     unsigned short int YID,
                     unsigned short int PORT_ID)
        : IPriorityGenerator(mn,numReqs_Grants,XID,YID,PORT_ID),
          update_register("PGRot_update_register"), Gdelayed("PGRot_Gdelayed",numReqs_Grants),
          nextP("PGRot_nextP",numReqs_Grants), Preg("PGRot_Preg",numReqs_Grants)
{

    unsigned short int i;
    SC_METHOD(p_gdelayed);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_update_register);
    for( i = 0; i < numReqs_Grants; i++) {
        sensitive << i_GRANTS[i] << Gdelayed[i];
    }

    SC_METHOD(p_nextp);
    for( i = 0; i < numReqs_Grants; i++) {
        sensitive << i_GRANTS[i];
    }

    SC_METHOD(p_preg);
    sensitive << i_CLK.pos() << i_RST;

    SC_METHOD(p_outputs);
    for( i = 0; i < numReqs_Grants; i++) {
        sensitive << Preg[i];
    }

}

////////////////////////////////////////////////////////////////////////////////
void PG_Rotative::p_gdelayed()
////////////////////////////////////////////////////////////////////////////////
// It's just a d-type register that holds the state of G for one clock cycle
{
    if (i_RST.read())
        for(unsigned short int i = 0; i < numPorts; i++)
            Gdelayed[i].write(0);
    else
        for(unsigned short int i = 0; i < numPorts; i++)
            Gdelayed[i].write(i_GRANTS[i].read());
}

////////////////////////////////////////////////////////////////////////////////
void PG_Rotative::p_update_register()
////////////////////////////////////////////////////////////////////////////////
// It just implements a parameterizable OR which detect if any request was
// granted in the last cycle. In this case, it enables the priority register
// to update its state.
{
//    sc_vector<bool> v_GRANTING("PGRot_v_GRANTING",numPorts);
//    sc_vector<bool> v_G("PGRot_v_G",numPorts);
//    sc_vector<bool> v_Gdelayed("PGRot_v_Gdelayed",numPorts);
    std::vector<bool> v_GRANTING(numPorts);
    std::vector<bool> v_G(numPorts);
    std::vector<bool> v_Gdelayed(numPorts);
//    sc_uint<N> granting;       // A request was granted
//    sc_uint<N> G_tmp;		   // A copy of G for bit-level operations
//    sc_uint<N> Gdelayed_tmp;   // A copy of Gdelayed for bit-level ops
    bool update_register_tmp;  // A temp. var. used to calculate update_register
    unsigned short int i;               // A variable for loops

    for(i = 0; i < numPorts; i++) {
        v_G[i] = i_GRANTS[i].read();
        v_Gdelayed[i] = Gdelayed[i].read();
    }

//    G_tmp = G.read();
//    Gdelayed_tmp = Gdelayed.read();

    // It first determines if there exists any request that was granted in the
    // last cycle.
    for (i = 0; i < numPorts; i++) {
        v_GRANTING[i] = v_G[i] and (not v_Gdelayed[i]);
//        granting[i] = G_tmp[i] and (not Gdelayed_tmp[i]);
    }

    // Then, it makes an OR operation among all the granting(i) bits
    update_register_tmp = 0;
    for (i = 0; i < numPorts; i++) {
        update_register_tmp = update_register_tmp | v_GRANTING[i];
//        update_register_tmp = update_register_tmp | granting[i];
    }

    update_register.write(update_register_tmp);
}


////////////////////////////////////////////////////////////////////////////////
void PG_Rotative::p_nextp()
////////////////////////////////////////////////////////////////////////////////
// It determines the next priority order by rotating 1x left the current
// priority status. Ex. If Preg="0001", then, nextP="0010". Such rotation will
// ensure that the current priority (e.g. R(0)) will have the lowest
// priority level at the next arbitration cycle (e.g. P(1)>P(2)> P(3)>P(0)).
{
//    sc_uint<N> Preg_tmp;    // A copy of Preg for bit-level operations
//    sc_uint<N> nextP_tmp;   // A temp. var. used to calculate nextP
//    sc_vector<bool> v_Preg("PGRot_v_Preg",numPorts);
//    sc_vector<bool> v_NEXT_P("PGRot_v_NEXT_P",numPorts);
    std::vector<bool> v_Preg(numPorts);
    std::vector<bool> v_NEXT_P(numPorts);
    unsigned short int i;

//    Preg_tmp = Preg.read();
    for( i = 0; i < numPorts; i++) {
        v_Preg[i] = Preg[i].read();
    }

//    nextP_tmp[0] = Preg_tmp[N-1];
    v_NEXT_P[0] = v_Preg[numPorts-1];

    for ( i = 1; i < numPorts; i++ ) {
//        nextP_tmp[i] = Preg_tmp[i-1];
        v_NEXT_P[i] = v_Preg[i-1];
    }

    for( i = 0; i < numPorts; i++) {
//        nextP.write(nextP_tmp);
        nextP[i].write(v_NEXT_P[i]);
    }
}


////////////////////////////////////////////////////////////////////////////////
void PG_Rotative::p_preg()
////////////////////////////////////////////////////////////////////////////////
// It is reset with bit 0 in 1 and the others in 0, and is updated at each
// arbitration cycle (after a request is grant) with the value determined
// for nextP.
{
//    sc_uint<N> nextP_tmp;   // A copy of nextP_tmp for bit-level operations
//    sc_uint<N> Preg_tmp;    // A temp. var. used to calculate Preg
//    sc_vector<bool> v_NEXT_P("PGRot_v_NEXT_P",numPorts);
//    sc_vector<bool> v_Preg("PGRot_v_Preg",numPorts);
    std::vector<bool> v_NEXT_P(numPorts);
    std::vector<bool> v_Preg(numPorts);

    unsigned short int i;

//    Preg_tmp 	= Preg.read();
//    nextP_tmp	= nextP.read();
    for( i = 0; i < numPorts; i++) {
        v_Preg[i] = Preg[i].read();
        v_NEXT_P[i] = nextP[i].read();
    }

    if (i_RST.read()) {
//        Preg_tmp[0] = 1;
        v_Preg[0] = 1;
    } else {
        if (update_register.read()) {
//            Preg_tmp[0] = nextP_tmp[0];
            v_Preg[0] = v_NEXT_P[0];
        }
    }

    for ( i = 1; i < numPorts; i++) {
        if (i_RST.read()) {
//            Preg_tmp[i] = 0;
            v_Preg[i] = 0;
        } else {
            if (update_register.read()) {
//                Preg_tmp[i] = nextP_tmp[i];
                v_Preg[i] = v_NEXT_P[i];
            }
        }
    }

//    Preg.write(Preg_tmp);
    for( i = 0; i < numPorts; i++) {
        Preg[i].write(v_Preg[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////
void PG_Rotative::p_outputs()
////////////////////////////////////////////////////////////////////////////////
// It updates the output
{
//    P.write(Preg.read());
    for(unsigned short int i = 0; i < numPorts; i++) {
        o_PRIORITIES[i].write( Preg[i].read() );
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
    SS_EXP IPriorityGenerator* new_PG(sc_simcontext* simcontext,
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
    SS_EXP void delete_PG(IPriorityGenerator* pg) {
        delete pg;
    }
}

