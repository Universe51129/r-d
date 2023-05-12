/*
    File:       QTSSRawFileModule.h

    Contains:   A module that returns the entire contents of a file to the client.
                Only does this if the suffix of the file is .raw
*/

#ifndef __QTSS_RAW_FILE_MODULE_H__
#define __QTSS_RAW_FILE_MODULE_H__

#include "QTSS.h"

extern "C"
{
    EXPORT QTSS_Error QTSSRawFileModule_Main(void* inPrivateArgs);
}

#endif // __QTSS_RAW_FILE_MODULE_H__


