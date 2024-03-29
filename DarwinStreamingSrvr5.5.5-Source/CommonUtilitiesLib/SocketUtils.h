#ifndef __SOCKETUTILS_H__
#define __SOCKETUTILS_H__

#ifndef __Win32__
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#endif

#include "ev.h"

#include "OSHeaders.h"
#include "MyAssert.h"
#include "StrPtrLen.h"
#include "OSMutex.h"

#ifdef __solaris__
    #ifndef INADDR_NONE
        #define INADDR_NONE     0xffffffff     
    #endif
#endif

class SocketUtils
{
    public:

        //initialize before using any socket functions.
        static void Initialize(Bool16 lookupDNSName = true);
        
        //static utility routines
        static Bool16   IsMulticastIPAddr(UInt32 inAddress);
        static Bool16   IsLocalIPAddr(UInt32 inAddress);

        static void ConvertAddrToString(const struct in_addr& theAddr, StrPtrLen* outAddr);
        
        static UInt32 ConvertStringToAddr(const char* inAddr);
        
        //You can get at all the IP addrs and DNS names on this machine this way
        static UInt32       GetNumIPAddrs() { return sNumIPAddrs; }
        static inline UInt32        GetIPAddr(UInt32 inAddrIndex);
        static inline StrPtrLen*    GetIPAddrStr(UInt32 inAddrIndex);
        static inline StrPtrLen*    GetDNSNameStr(UInt32 inDNSIndex);
            
    private:

#ifndef __Win32__
        static Bool16 IncrementIfReqIter(char** inIfReqIter, ifreq* ifr);
#endif
        //For storing relevent information about each IP interface
        struct IPAddrInfo
        {
            UInt32      fIPAddr;
            StrPtrLen   fIPAddrStr;
            StrPtrLen   fDNSNameStr;
        };
        
        static IPAddrInfo*              sIPAddrInfoArray;
        static UInt32                   sNumIPAddrs;
        static OSMutex                  sMutex;
};

inline UInt32   SocketUtils::GetIPAddr(UInt32 inAddrIndex)
{
    Assert(sIPAddrInfoArray != NULL);
    Assert(inAddrIndex < sNumIPAddrs);
    return sIPAddrInfoArray[inAddrIndex].fIPAddr;
}

inline StrPtrLen*   SocketUtils::GetIPAddrStr(UInt32 inAddrIndex)
{
    Assert(sIPAddrInfoArray != NULL);
    Assert(inAddrIndex < sNumIPAddrs);
    return &sIPAddrInfoArray[inAddrIndex].fIPAddrStr;
}

inline StrPtrLen*   SocketUtils::GetDNSNameStr(UInt32 inDNSIndex)
{
    Assert(sIPAddrInfoArray != NULL);
    Assert(inDNSIndex < sNumIPAddrs);
    return &sIPAddrInfoArray[inDNSIndex].fDNSNameStr;
}

#endif 

