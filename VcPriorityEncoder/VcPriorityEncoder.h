/*
--------------------------------------------------------------------------------
PROJECT: SoCIN Simulator
MODULE : VcPriorityEncoder
FILE   : VcPriorityEncoder.h
--------------------------------------------------------------------------------
DESCRIPTION: Priority Encoder for virtual channels
--------------------------------------------------------------------------------
AUTHORS: Laboratory of Embedded and Distributed Systems (LEDS - UNIVALI)
CONTACT: Prof. Cesar Zeferino (zeferino@univali.br)
-------------------------------- Reviews ---------------------------------------
| Date       - Version - Author                      | Description
--------------------------------------------------------------------------------
| 03/10/2016 - 1.0     - Eduardo Alves da Silva      | Reuse from SoCIN 8VC
--------------------------------------------------------------------------------
*/
#ifndef __VC_PRIORITY_ENCODER_H__
#define __VC_PRIORITY_ENCODER_H__

#include "../SoCINModule.h"
#include "../SoCINDefines.h"

/*!
 * \brief The PriorityEncoder class implements the priority encoder
 * for dynamic virtual channels
 */
class VcPriorityEncoder : public SoCINModule {
protected:
    unsigned short numPorts;
    unsigned short widthVcSelector;
public:

    sc_vector<sc_in<bool> >  i_REQUEST;// Requests
    sc_vector<sc_out<bool> > o_GRANT;  // Grant

    // Module's process
    void p_PRIORITY();

    SC_HAS_PROCESS(VcPriorityEncoder);
    VcPriorityEncoder(sc_module_name mn, unsigned short numReqs);

    ModuleType moduleType() const { return SoCINModule::PriorityEncoder; }
    const char* moduleName() const { return "VcPriorityEncoder"; }

    ~VcPriorityEncoder();
};

inline VcPriorityEncoder::VcPriorityEncoder(sc_module_name mn, unsigned short numReqs)
    : SoCINModule(mn),
      numPorts(numReqs),
      widthVcSelector( (unsigned short)ceil(log2(numReqs)) ),
      i_REQUEST("VcPriorityEncoder_iREQUEST",numReqs),
      o_GRANT("VcPriorityEncoder_oGRANT")
{
    o_GRANT.init(widthVcSelector);
    SC_METHOD(p_PRIORITY);
    for( unsigned short i = 0; i < numPorts; i++ ) {
        sensitive << i_REQUEST[i];
    }
}

inline VcPriorityEncoder::~VcPriorityEncoder() {}

inline void VcPriorityEncoder::p_PRIORITY() {
    unsigned short i,x;

    UIntVar v_REQUEST(0,numPorts); // (Valor, Width)
    UIntVar v_GRANT(0,widthVcSelector);

    bool v_TMP;

    for( i = numPorts-1; i > 0; i-- ) { // Index 0 is ignored
        v_TMP = i_REQUEST[i].read();
        for(x = i-1; x < numPorts; x--) {
            v_TMP = v_TMP and (not i_REQUEST[x].read());
        }
        v_REQUEST[i] = v_TMP;
    }
    v_REQUEST[0] = i_REQUEST[0].read();
/* // Original code to priority encoder of 8 virtual channels - 8 requests 1-bit -> grant 3-bit
    sc_uint<3> t_sel;
    bool t_REQ1;
    bool t_REQ2;
    bool t_REQ3;
    bool t_REQ4;
    bool t_REQ5;
    bool t_REQ6;
    bool t_REQ7;

    t_REQ1=( not i_REQ0.read()  and i_REQ1.read());
    t_REQ2=((not i_REQ0.read()) and (not i_REQ1.read()) and i_REQ2.read());
    t_REQ3=((not i_REQ0.read()) and (not i_REQ1.read()) and (not i_REQ2.read()) and i_REQ3.read());
    t_REQ4=((not i_REQ0.read()) and (not i_REQ1.read()) and (not i_REQ2.read()) and (not i_REQ3.read()) and i_REQ4.read());
    t_REQ5=((not i_REQ0.read()) and (not i_REQ1.read()) and (not i_REQ2.read()) and (not i_REQ3.read()) and (not i_REQ4.read()) and i_REQ5.read());
    t_REQ6=((not i_REQ0.read()) and (not i_REQ1.read()) and (not i_REQ2.read()) and (not i_REQ3.read()) and (not i_REQ4.read()) and (not i_REQ5.read()) and i_REQ6.read());
    t_REQ7=((not i_REQ0.read()) and (not i_REQ1.read()) and (not i_REQ2.read()) and (not i_REQ3.read()) and (not i_REQ4.read()) and (not i_REQ5.read()) and (not i_REQ6.read()) and i_REQ7.read());

    t_sel[0] = (t_REQ1 | t_REQ3 | t_REQ5 | t_REQ7);
    t_sel[1] = (t_REQ2 | t_REQ3 | t_REQ6 | t_REQ7);
    t_sel[2] = (t_REQ4 | t_REQ5 | t_REQ6 | t_REQ7);

    o_GNT.write(t_sel);
*/
    // WARNING: Verificar como escalar a solução para outras dimensões
    // RT solution - exclusive for 8 virtual channels | 8 requests
    if(numPorts==8) {
        v_GRANT[0] = v_REQUEST[1] | v_REQUEST[3] | v_REQUEST[5] | v_REQUEST[7];
        v_GRANT[1] = v_REQUEST[2] | v_REQUEST[3] | v_REQUEST[6] | v_REQUEST[7];
        v_GRANT[2] = v_REQUEST[4] | v_REQUEST[5] | v_REQUEST[6] | v_REQUEST[7];
    } else {
        // Variable number of virtual channels != 8
        for(i = 0; i < numPorts; i++) {
            if( v_REQUEST[i] == 1 ) { // Discover what request is high [i]
                v_GRANT = i;
                break;
            }
        }
    }

    std::cout << "\n[Pri_Encoder] - Req:  "
              << v_REQUEST.to_string(SC_BIN_US)
              << " | Grant: " << v_GRANT.to_string(SC_BIN_US);
    for(i = 0; i < widthVcSelector; i++) {
        o_GRANT[i].write(v_GRANT[i]);
    }

}

#endif // __VC_PRIORITY_ENCODER_H__
