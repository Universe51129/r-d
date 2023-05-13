#ifndef SDPGen_H
#define SDPGen_H

#include <stdio.h>
#include <stdlib.h>
#include "SafeStdLib.h"
#include <fcntl.h>
#include <string.h>

#ifndef __Win32__
    #include <sys/types.h>
    #include <sys/time.h>
    #include <sys/errno.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <sys/utsname.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/ioctl.h>
    #include <unistd.h>
    #include <netinet/in.h>
    #include <net/if.h>
#endif

#include "playlist_SimpleParse.h"
#include "QTRTPFile.h"

class SDPGen
{
    enum {eMaxSDPFileSize = 10240};
    enum {ePath_Separator = '/', eExtension_Separator = '.'};

    private:
        char *fSDPFileContentsBuf;
        
    protected: 
        QTRTPFile       fRTPFile;
                bool    fKeepTracks;
                bool    fAddIndexTracks;
                short   AddToBuff(char *aSDPFile, short currentFilePos, char *chars);
                UInt32  RandomTime(void);
                short   GetNoExtensionFileName(char *pathName, char *result, short maxResult);
                char    *Process(   char *sdpFileName, 
                                    char * basePort, 
                                    char *ipAddress, 
                                    char *anSDPBuffer, 
                                    char *startTime,
                                    char *endTime,  
                                    char *isDynamic,
                                    char *ttl,
                                    int *error
                                );
        
    public: 
             SDPGen() : fSDPFileContentsBuf(NULL), fKeepTracks(false),fAddIndexTracks(false), fSDPFileCreated(false),fClientBufferDelay(0.0) {  QTRTPFile::Initialize(); };
            ~SDPGen() { if (fSDPFileContentsBuf) delete fSDPFileContentsBuf; };
            void KeepSDPTracks(bool enabled) { fKeepTracks = enabled;} ;
            void AddIndexTracks(bool enabled) { fAddIndexTracks = enabled;} ;
            void SetClientBufferDelay(Float32 bufferDelay) { fClientBufferDelay = bufferDelay;} ;
            int Run(  char *movieFilename
                , char *sdpFilename
                , char *basePort
                , char *ipAddress
                , char *buff
                , short buffSize
                , bool overWriteSDP
                , bool forceNewSDP
                , char *startTime
                , char *endTime
                , char *isDynamic
                , char *ttl
                ); 
                
            bool    fSDPFileCreated;
            Float32 fClientBufferDelay;
};

#endif

