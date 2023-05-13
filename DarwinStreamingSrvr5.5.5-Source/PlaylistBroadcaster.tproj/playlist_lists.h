#ifndef playlist_lists_H
#define playlist_lists_H


#include <stdio.h>
#include <stdlib.h>
#include "SafeStdLib.h"
#include <string.h>
#include <errno.h>


#ifndef __Win32__
    #include <sys/types.h>
    #include <fcntl.h>
#endif

#include "OSHeaders.h"
#include "playlist_array.h"
#include "playlist_elements.h"
#include "playlist_utils.h"

// SOCKET LIST

class SocketList : public ArrayList<UDPSocketPair> {
    
};

// MEDIA STREAM LIST

class MediaStreamList : public ArrayList<MediaStream> {

    protected:
    
    public:
        SInt16 UpdateStreams();
        void UpdateSenderReportsOnStreams();
        void SetUpStreamSSRCs();
        void StreamStarted(SInt64 startTime);
        void MovieStarted(SInt64 startTime);
        void MovieEnded(SInt64 endTime);
};

// SDP MEDIA LIST

class SDPMediaList : public ArrayList<TypeMap> {
        
};

#endif //playlist_lists_H
