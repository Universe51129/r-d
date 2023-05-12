/*
    File:       QTSSFileModule.h

    Contains:   Content source module that uses the QTFileLib to serve Hinted QuickTime
                files to clients. 
                    
*/

#ifndef _RTPRTPFILEMODULE_H_
#define _RTPRTPFILEMODULE_H_

#include "QTSS.h"

extern "C"
{
    EXPORT QTSS_Error QTSSRTPFileModule_Main(void* inPrivateArgs);
}

#endif //_RTPRTPFILEMODULE_H_
