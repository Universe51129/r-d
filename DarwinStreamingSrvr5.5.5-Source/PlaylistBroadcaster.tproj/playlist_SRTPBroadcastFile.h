#ifndef QTRTPBroadcastFile_H
#define QTRTPBroadcastFile_H

#include "OSHeaders.h"
#include "QTRTPFile.h"


#ifndef __Win32__
    #include <sys/stat.h>
#endif



class QTRTPBroadcastFile  : public QTRTPFile {


public:

    bool FindTrackSSRC( UInt32 SSRC);
    

};

#endif // SRTPBroadcastFile
