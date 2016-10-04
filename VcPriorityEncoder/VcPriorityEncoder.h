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

    for(i = 0; i < widthVcSelector; i++) {
        o_GRANT[i].write(v_GRANT[i]);
    }

}

#endif // __VC_PRIORITY_ENCODER_H__
