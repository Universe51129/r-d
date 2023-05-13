#ifndef __PREFSSOURCE_H__
#define __PREFSSOURCE_H__

#include "OSHeaders.h"

class PrefsSource
{
    public:
    
        virtual int     GetValue(const char* inKey, char* ioValue) = 0;
        virtual int     GetValueByIndex(const char* inKey, UInt32 inIndex, char* ioValue) = 0;

};

#endif
