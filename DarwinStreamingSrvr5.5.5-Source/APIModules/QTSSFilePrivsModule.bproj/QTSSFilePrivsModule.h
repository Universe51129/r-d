/* 
    File:       QTSSFILEPRIVSMODULE.h

    Contains:   Module that handles and file system authorization
                    
    

*/

#ifndef _QTSSFILEPRIVSMODULE_H_
#define _QTSSFILEPRIVSMODULE_H_

#include "QTSS.h"

extern "C"
{
    EXPORT QTSS_Error QTSSFilePrivsModule_Main(void* inPrivateArgs);
}

#endif //_QTSSFILEPRIVSMODULE_H_


