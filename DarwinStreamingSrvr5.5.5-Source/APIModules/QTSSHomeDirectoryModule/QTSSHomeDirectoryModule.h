/*
        File:       QTSSHomeDirectoryModule.h

        Contains:   Module that expands ~ in request URLs to home directories
*/

#ifndef __QTSS_HOMEDIRECTORY_MODULE_H__
#define __QTSS_HOMEDIRECTORY_MODULE_H__

#include "QTSS.h"

extern "C"
{
    QTSS_Error QTSSHomeDirectoryModule_Main(void* inPrivateArgs);
}

#endif // __QTSS_HOMEDIRECTORY_MODULE_H__
