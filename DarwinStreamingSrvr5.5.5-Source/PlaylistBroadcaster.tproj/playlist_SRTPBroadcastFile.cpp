#include <stdio.h>
#include <stdlib.h>
#include "SafeStdLib.h"
#include <string.h>
#include "playlist_QTRTPBroadcastFile.h"



bool QTRTPBroadcastFile::FindTrackSSRC( UInt32 SSRC)
{
    // General vars
    RTPTrackListEntry   *ListEntry;


    //
    // Find the specified track.
    for( ListEntry = fFirstTrack; ListEntry != NULL; ListEntry = ListEntry->NextTrack ) 
    {
        // Check for matches.
        if( ListEntry->SSRC == SSRC ) 
        {   return true;
        }
    }

    // The search failed.
    return false;
}


