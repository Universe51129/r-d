#ifndef __Win32__
#include <sys/types.h>
#include <sys/socket.h>

#if __solaris__
#include "SocketUtils.h"
#endif

#if NEED_SOCKETBITS
#if __GLIBC__ >= 2
#include <bits/socket.h>
#else
#include <socketbits.h>
#endif
#endif
#endif

#include <errno.h>
#include "UDPSocket.h"
#include "OSMemory.h"

#ifdef USE_NETLOG
#include <netlog.h>
#endif

UDPSocket::UDPSocket(Task* inTask, UInt32 inSocketType)
: Socket(inTask, inSocketType), fDemuxer(NULL)
{
    if (inSocketType & kWantsDemuxer)
        fDemuxer = NEW UDPDemuxer();
        
    //setup msghdr
    ::memset(&fMsgAddr, 0, sizeof(fMsgAddr));
}


OS_Error
UDPSocket::SendTo(UInt32 inRemoteAddr, UInt16 inRemotePort, void* inBuffer, UInt32 inLength)
{
    Assert(inBuffer != NULL);
    
    struct sockaddr_in  theRemoteAddr;
    theRemoteAddr.sin_family = AF_INET;
    theRemoteAddr.sin_port = htons(inRemotePort);
    theRemoteAddr.sin_addr.s_addr = htonl(inRemoteAddr);

#ifdef __sgi__
	int theErr = ::sendto(fFileDesc, inBuffer, inLength, 0, (sockaddr*)&theRemoteAddr, sizeof(theRemoteAddr));
#else
	int theErr = ::sendto(fFileDesc, (char*)inBuffer, inLength, 0, (sockaddr*)&theRemoteAddr, sizeof(theRemoteAddr));
#endif

    if (theErr == -1)
        return (OS_Error)OSThread::GetErrno();
    return OS_NoErr;
}

OS_Error UDPSocket::RecvFrom(UInt32* outRemoteAddr, UInt16* outRemotePort,
                            void* ioBuffer, UInt32 inBufLen, UInt32* outRecvLen)
{
    Assert(outRecvLen != NULL);
    Assert(outRemoteAddr != NULL);
    Assert(outRemotePort != NULL);
    
#if __Win32__ || __osf__  || __sgi__ || __hpux__
    int addrLen = sizeof(fMsgAddr);
#else
    socklen_t addrLen = sizeof(fMsgAddr);
#endif

#ifdef __sgi__
	SInt32 theRecvLen = ::recvfrom(fFileDesc, ioBuffer, inBufLen, 0, (sockaddr*)&fMsgAddr, &addrLen);
#else
    SInt32 theRecvLen = ::recvfrom(fFileDesc, (char*)ioBuffer, inBufLen, 0, (sockaddr*)&fMsgAddr, &addrLen);
#endif

    if (theRecvLen == -1)
        return (OS_Error)OSThread::GetErrno();
    
    *outRemoteAddr = ntohl(fMsgAddr.sin_addr.s_addr);
    *outRemotePort = ntohs(fMsgAddr.sin_port);
    Assert(theRecvLen >= 0);
    *outRecvLen = (UInt32)theRecvLen;
    return OS_NoErr;        
}

OS_Error UDPSocket::JoinMulticast(UInt32 inRemoteAddr)
{
    struct ip_mreq  theMulti;
        UInt32 localAddr = fLocalAddr.sin_addr.s_addr; 

#if __solaris__
    if( localAddr == htonl(INADDR_ANY) )
         localAddr = htonl(SocketUtils::GetIPAddr(0));
#endif
    theMulti.imr_multiaddr.s_addr = htonl(inRemoteAddr);
    theMulti.imr_interface.s_addr = localAddr;
    int err = setsockopt(fFileDesc, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&theMulti, sizeof(theMulti));
    if (err == -1)
         return (OS_Error)OSThread::GetErrno();
    else
         return OS_NoErr;
}

OS_Error UDPSocket::SetTtl(UInt16 timeToLive)
{
    u_char  nOptVal = (u_char)timeToLive;
    int err = setsockopt(fFileDesc, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&nOptVal, sizeof(nOptVal));
    if (err == -1)
        return (OS_Error)OSThread::GetErrno();
    else
        return OS_NoErr;    
}

OS_Error UDPSocket::SetMulticastInterface(UInt32 inLocalAddr)
{
    
    in_addr theLocalAddr;
    theLocalAddr.s_addr = inLocalAddr;
    int err = setsockopt(fFileDesc, IPPROTO_IP, IP_MULTICAST_IF, (char*)&theLocalAddr, sizeof(theLocalAddr));
    AssertV(err == 0, OSThread::GetErrno());
    if (err == -1)
        return (OS_Error)OSThread::GetErrno();
    else
        return OS_NoErr;    
}

OS_Error UDPSocket::LeaveMulticast(UInt32 inRemoteAddr)
{
    struct ip_mreq  theMulti;
    theMulti.imr_multiaddr.s_addr = htonl(inRemoteAddr);
    theMulti.imr_interface.s_addr = htonl(fLocalAddr.sin_addr.s_addr);
    int err = setsockopt(fFileDesc, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&theMulti, sizeof(theMulti));
    if (err == -1)
        return (OS_Error)OSThread::GetErrno();
    else
        return OS_NoErr;    
}
