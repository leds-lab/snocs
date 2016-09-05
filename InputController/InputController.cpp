#include "InputController.h"

InputController::InputController(sc_module_name mn,
                                 unsigned short nPorts,
                                 RequestRegister *reqReg,
                                 IRouting *routing,
                                 unsigned short XID,
                                 unsigned short YID,
                                 unsigned short PORT_ID)
    : SoCINModule(mn) , numPorts(nPorts),
      i_CLK("IRouting_iCLK"),
      i_RST("IRouting_iRST"),
      i_READ("IRouting_iREAD"),
      i_READ_OK("IRouting_iREAD_OK"),
      i_DATA("IRouting_iDATA"),
      i_IDLE("IRouting_iIDLE",nPorts),
      o_REQUEST("IRouting_oREQUEST",nPorts),
      o_REQUESTING("IRouting_oREQUESTING"),
      w_REQUEST("IRouting_wREQUEST",nPorts),
      XID(XID), YID(YID), PORT_ID(PORT_ID),
      u_REQ_REG(reqReg), u_ROUTING(routing)
{
    // Binding ports
    // Routing
    u_ROUTING->i_READ_OK(i_READ_OK);
    u_ROUTING->i_DATA(i_DATA);
    u_ROUTING->i_IDLE(i_IDLE);
    u_ROUTING->o_REQUEST(w_REQUEST);

    // Request register
    u_REQ_REG->i_CLK(i_CLK);
    u_REQ_REG->i_RST(i_RST);
    u_REQ_REG->i_DATA(i_DATA);
    u_REQ_REG->i_READ(i_READ);
    u_REQ_REG->i_READ_OK(i_READ_OK);
    u_REQ_REG->i_REQUEST(w_REQUEST);
    u_REQ_REG->o_REQUEST(o_REQUEST);
    u_REQ_REG->o_REQUESTING(o_REQUESTING);
}

InputController::~InputController() {
    // TODO:  Verificar desalocação
    delete u_REQ_REG;
}
