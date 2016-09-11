#include "ParIS.h"


ParIS::ParIS(sc_module_name mn,
             unsigned short nPorts,
             unsigned short XID,
             unsigned short YID)
    : IRouter(mn,nPorts,XID,YID)
{

}
