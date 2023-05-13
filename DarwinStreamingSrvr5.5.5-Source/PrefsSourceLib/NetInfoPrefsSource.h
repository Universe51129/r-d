#ifndef __NETINFOPREFSSOURCE_H__
#define __NETINFOPREFSSOURCE_H__

#include "PrefsSource.h"
#include "OSHeaders.h"

class NetInfoPrefsSource : public PrefsSource
{
    public:
    
        NetInfoPrefsSource();
        virtual ~NetInfoPrefsSource() {}
    
        virtual int     GetValue(const char* inKey, char* ioValue);
        virtual int     GetValueByIndex(const char* inKey, UInt32 inIndex, char* ioValue);


        void    SetValue(char* inKey, char* inValue);
        void    SetValueByIndex(char* inKey, char* inValue, UInt32 inIndex);
};

#endif //__NETINFOPREFSSOURCE_H__
