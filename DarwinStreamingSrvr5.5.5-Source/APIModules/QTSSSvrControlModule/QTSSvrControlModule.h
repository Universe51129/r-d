/*
    File:       QTSSvrControlModule.h

    Contains:   A module that supports the MacOSX Server Server Control interface.
                Allows the QTSS Administration application to send messages to the server.

*/

#ifndef _QTSSERVERCONTROLMODULE_H_
#define _QTSSERVERCONTROLMODULE_H_

#include "QTSS.h"

extern "C"
{
    EXPORT QTSS_Error QTSSvrControlModule_Main(void* inPrivateArgs);
}

#endif //_RTSPSERVERCONTROLMODULE_H_
