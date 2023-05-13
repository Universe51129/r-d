#include "playlist_lists.h"
#include "OS.h"
#include "playlist_utils.h"

// MEDIA STREAM LIST

void MediaStreamList::SetUpStreamSSRCs()
{
    UInt32          ssrc;
    MediaStream*    setMediaStreamPtr;
    MediaStream*    aMediaStreamPtr;
    bool            found_duplicate;

    for (int i = 0; i < Size(); i++)
    {   
        setMediaStreamPtr = SetPos(i);
        if (setMediaStreamPtr != NULL) do 
        {  
            ssrc = PlayListUtils::Random() + ( (UInt32) OS::Milliseconds() ); // get a new ssrc
            aMediaStreamPtr = Begin();      // start at the beginning of the stream list
            found_duplicate = false;        // default is don't loop
  
            while (aMediaStreamPtr != NULL) //check all the streams for a duplicate
            {
                if (aMediaStreamPtr->fData.fInitSSRC == ssrc) // it is a duplicate
                {   found_duplicate = true; // set to loop: try a new ssrc 
                    break;
                }

                aMediaStreamPtr = Next(); // keep checking for a duplicate
            }
               
            if (!found_duplicate) // no duplicates found so keep this ssrc
                setMediaStreamPtr->fData.fInitSSRC = ssrc;
                
        } while (found_duplicate); // we have a duplicate ssrc so find another one
    }
}


void MediaStreamList::StreamStarted(SInt64 startTime)
{
    for ( MediaStream *theStreamPtr = Begin(); (theStreamPtr != NULL) ; theStreamPtr = Next() ) 
    {               
        theStreamPtr->StreamStart(startTime);
    }   
}

void MediaStreamList::MovieStarted(SInt64 startTime)
{
    for ( MediaStream *theStreamPtr = Begin(); (theStreamPtr != NULL) ; theStreamPtr = Next() ) 
    {               
        theStreamPtr->MovieStart(startTime);
    } 
}

void MediaStreamList::MovieEnded(SInt64 endTime)
{
    for ( MediaStream *theStreamPtr = Begin(); (theStreamPtr != NULL) ; theStreamPtr = Next() ) 
    {               
        theStreamPtr->MovieEnd(endTime);
    } 
}


SInt16 MediaStreamList::UpdateStreams()
{
    SInt16 err = 0;
    for ( MediaStream *theStreamPtr = Begin(); (theStreamPtr != NULL) ; theStreamPtr = Next() ) 
    {               
        theStreamPtr->ReceiveOnPorts();
    };
    
    return err;
}

void MediaStreamList::UpdateSenderReportsOnStreams()
{
    SInt64 theTime = PlayListUtils::Milliseconds();
    for ( MediaStream *theStreamPtr = Begin(); (theStreamPtr != NULL) ; theStreamPtr = Next() ) 
    {               
        (void) theStreamPtr->UpdateSenderReport(theTime);
    };
    
}

