/*
    File:       QTSSFlowControlModule.h

    Contains:   Uses information in RTCP packers to throttle back the server
                when it's pumping out too much data to a given client
 
    
*/

#ifndef _QTSSFLOWCONTROLMODULE_H_
#define _QTSSFLOWCONTROLMODULE_H_

#include "QTSS.h"

extern "C"
{
    EXPORT QTSS_Error QTSSFlowControlModule_Main(void* inPrivateArgs);
}

#endif //_QTSSFLOWCONTROLMODULE_H_
