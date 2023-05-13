#ifndef playlist_utils_H
#define playlist_utils_H

#include <stdio.h>
#include <stdlib.h>
#include "SafeStdLib.h"
#include <string.h>
#include <errno.h>
#ifndef __Win32__
    #include <sys/types.h>
    #include <sys/time.h>
    #include <sys/stat.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>

    #include <fcntl.h>
#endif

#include "OSHeaders.h"
#include "OS.h"

#define Log_Enabled 0

#if Log_Enabled
    
    void LogFileOpen(void);
    void LogFileClose(void);
    bool TimeToSwitch(int len);
    void WriteToLog(void *data, int len);
    void WritePacketToLog(void *data, int len);
    void WritToBuffer(void *data, int len);
    void LogBuffer(void);
    void PrintLogBuffer(bool log);
    void LogNum(void);
    void LogFloat(char *str, float num, char *str2= "\0");
    void LogInt(char *str, long num, char *str2= "\0");
    void LogUInt (char *str, unsigned long num, char *str2 = "\0");
    void LogStr(char *str);
    
#else
    #define LogFileOpen()
    #define LogFileClose()
    #define TimeToSwitch(len)
    #define WriteToLog(data,len)
    #define WritePacketToLog(data,len)
    #define WritToBuffer(data, len)
    #define LogBuffer()
    #define PrintLogBuffer(log)
    #define LogFloat(str,num,str2)  
    #define LogInt(str,num,str2)
    #define LogUInt(str,num,str2) 
    #define LogStr(str) 
    #define LogNum() 
#endif

#define kFixed64 ( (UInt64) 1 << 32)

class PlayListUtils 
{
    public:
        PlayListUtils();
        
                    
        enum {eMicro = 1000000, eMilli = 1000};
        static SInt64   sInitialMsecOffset;
        static UInt32   sRandomNum;
        static  void    InitRandom() {};
        static void   Initialize();
        static UInt32 Random();
        static SInt64 Milliseconds() {return OS::Milliseconds() - sInitialMsecOffset;};
        static SInt64 TimeMilli_To_1900Fixed64Secs(SInt64 inMilliseconds) { return OS::TimeMilli_To_1900Fixed64Secs(OS::TimeMilli_To_UnixTimeMilli(sInitialMsecOffset + inMilliseconds)); }
        static SInt64 TimeMilli_To_Fixed64Secs(SInt64 inMilliseconds) { return OS::TimeMilli_To_Fixed64Secs(inMilliseconds); }
};

#endif  //playlist_utils_H
