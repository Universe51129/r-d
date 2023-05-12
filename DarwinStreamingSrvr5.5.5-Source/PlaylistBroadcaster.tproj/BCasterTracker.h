#ifndef __BCASTERTRACKER__
#define __BCASTERTRACKER__

#include <stdio.h>
#include "TrackingElement.h"
#include "PLDoubleLinkedList.h"

class BCasterTracker 
{
    public:
                                BCasterTracker( const char* fName );
        virtual                 ~BCasterTracker();
    
        int                     RemoveByProcessID( pid_t pid );
        int                     Remove( UInt32 itemID );
        int                     Add( pid_t pid, const char* bcastList );
        int                     Save();
        void                    Show();
        static bool         IsProcessRunning( pid_t pid );
        bool                    IsOpen();
        
    protected:

        enum {  kTrackerLineBuffSize = 512 };
        
        FILE*                   mTrackerFile;
        long                    mEofPos;
        
        PLDoubleLinkedList<TrackingElement> mTrackingList;
        
};

void TestBCasterTracker(int x);

#endif
