#include "BCasterTracker.h"
#include "MyAssert.h"

#include "Trim.h"
#include "GetWord.h"
#include "OSThread.h"

#include <stdlib.h>
#include "SafeStdLib.h"
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#if !(defined(__solaris__) || defined(__osf__) || defined(__hpux__))
	#include <sys/sysctl.h>
#endif
#include <sys/time.h>
#include <signal.h>

char    gTrackerFileTempDataPath[256];

void TestBCasterTracker(int x )
{

    BCasterTracker  tracker( "trackerfile" );
    
    
    if ( tracker.IsOpen() )
    {
        if ( x > - 1 )
        {   int     error;
        
            error = tracker.Remove( x );
            
            if ( error )    // remove the xth item from the list.
                sss_printf( "Playlist Broadcast (%li) not found.\n", (long)x );
            else
                tracker.Save();
        }
        else
        {   
            tracker.Show();
        }
    }
    else
        sss_printf("PlaylistBroadcaster trackerfile open FAILED.\n");
}

static void ShowElement( PLDoubleLinkedListNode<TrackingElement>* ten,  void* userData)
{
    int*    showIndex = (int*)userData;
    char    *info;
    
    if ( BCasterTracker::IsProcessRunning( ten->fElement->mPID ) )
        info = "";
    else
        info = ", (not running)";
  
    sss_printf(  "[%3li] %s; pid: %li%s\n", (long)*showIndex, ten->fElement->mName,  (long)ten->fElement->mPID,  info );
    
    
    *(int*)userData = *showIndex + 1;
    

}


void BCasterTracker::Show()
{
    int     showIndex = 1;
    
    if ( mTrackingList.GetNumNodes() )
    {
        sss_printf( "\n" );
        sss_printf( "Current Playlist Broadcasts\n" );
        sss_printf( " ID   Description file; Process ID\n" );
        sss_printf( "----------------------------------\n" );
        
        // display the elements in the list
        mTrackingList.ForEach( ShowElement, &showIndex );
    }
    else
    {   sss_printf( "\n" );
        sss_printf( "- PlaylistBroadcaster: No Broadcasts running.\n" );
    }
    
}


bool BCasterTracker::IsProcessRunning( pid_t pid )
{
    bool                isRunning=false;

    // a no-grep version to find the pid
    
    char pidStr[32];
    sss_sprintf( pidStr, "%li",(long)pid);    
    
    char procPath[64] = "ps -p";
    ::strcat( procPath, pidStr);    

    FILE *inPipe = ::popen(procPath, "r");
    if (NULL == inPipe)
        return false;
        
    char inBuff[256] = "";
    while (!isRunning && ::fgets(inBuff, sizeof(inBuff), inPipe) != 0)
    {   if (::strstr(inBuff,pidStr) != NULL)
        {   isRunning = true;
            break;
        }
    }
    
    (void) ::pclose(inPipe);
        

    return isRunning;

}

bool IsProcessID( PLDoubleLinkedListNode<TrackingElement>*ten,  void* userData)
{

    pid_t       pid;
    bool        isProcID = false;
    
    pid = *(pid_t*)userData;
    
    if ( pid == ten->fElement->mPID )
        isProcID = true;

    return isProcID;

}


int BCasterTracker::RemoveByProcessID( pid_t pid )
{
    int                     error = -1;
    
    // remove the element with the given process ID from the tracking list.
        
    PLDoubleLinkedListNode<TrackingElement> *te;
    
    te = mTrackingList.ForEachUntil( IsProcessID, &pid );
    
    // remove that element
    if ( te )
    {   
        mTrackingList.RemoveNode(te);
        error = 0;
    }
    
    return error;

}

static bool IsElementID( PLDoubleLinkedListNode<TrackingElement> */*ten*/,  void* userData)
{

    UInt32*     showIndex = (UInt32*)userData;
    bool        isItemId = false;
    
    if ( *showIndex == 0 )
        isItemId = true;
    else
        *(int*)userData = *showIndex - 1;
    
    return isItemId;

}

int BCasterTracker::Remove( UInt32 itemID )
{

    int                     error = -1;
    UInt32                  itemIDIndex = itemID;

    // KILL the process that is associated with the item id in our list.
    // remove the element with the given index in the list from the tracking list.
    
    PLDoubleLinkedListNode<TrackingElement> *te;
    
    
    te = mTrackingList.ForEachUntil( IsElementID, &itemIDIndex );
    
    
    // remove that element, and kill the process
    if ( te )
    {
        if ( ::kill( te->fElement->mPID, SIGTERM ) == 0 )
        {   
            error = 0;
        }
        else
        {   
        
            error = OSThread::GetErrno();
        
            if ( error != ESRCH ) // no such process
            {     
            
            }
            else
            {   // this process already died, or was killed by other means.
                error = 0;
            }
                
        }
        
        if ( !error )
        {
            mTrackingList.RemoveNode(te);
        }
        
    }
        
    return error;
}


int BCasterTracker::Add( pid_t pid, const char* bcastList )
{
    /*
        add an entry to our tracking list
    */
    
    int     addErr = -1;
    
    
    TrackingElement* te = new TrackingElement( pid, bcastList );
    
    Assert( te );
    
    if ( te )
    {
        PLDoubleLinkedListNode<TrackingElement>* ten = new PLDoubleLinkedListNode<TrackingElement>( te );
        
        Assert( ten );
        
        if ( ten )
        {   mTrackingList.AddNodeToTail( ten );
            addErr = 0;
        }
        else
            delete te;
    }
    
    return addErr;

}



BCasterTracker::~BCasterTracker()
{
    // truncate the file to the desired size and close.
    
    if ( mTrackerFile != NULL )
    {   
        int  err = OSThread::GetErrno();
                
        if ( ::fseek( mTrackerFile, mEofPos, SEEK_SET ) < 0 )
            qtss_printf( "fseek at close eof(%li), err(%li), fileno(%li)\n", mEofPos, (long)err, (long)fileno(mTrackerFile) );
            
        if ( ::fflush( mTrackerFile ) )
            qtss_printf( "fflush at close eof(%li), err(%li), fileno(%li)\n", mEofPos, (long)err, (long)fileno(mTrackerFile) );

        if ( ::ftruncate( fileno(mTrackerFile), (off_t)mEofPos ) )
        {   
            qtss_printf( "ftruncate at close eof(%li), err(%li), fileno(%li)\n", mEofPos, (long)err, (long)fileno(mTrackerFile) );
        }
        
        (void)::fclose(  mTrackerFile  );
    }
}

static bool SaveElement( PLDoubleLinkedListNode<TrackingElement>* ten,  void* userData)
{
    FILE*   fp = (FILE*) userData;
    char    buff[512];
    
    qtss_sprintf( buff, "%li \"%s\"\n", (long)ten->fElement->mPID, ten->fElement->mName );

    if (::fputs( buff, fp ) < 0 )
        return true;
        
    return false;
    
}

int BCasterTracker::Save()
{   
/
    int error = -1;
    
    mEofPos = 0;
    
    if ( mTrackerFile != NULL )
    {
    
        if ( !::fseek( mTrackerFile, 0, SEEK_SET ) )
        {
            if ( !mTrackingList.ForEachUntil( SaveElement, mTrackerFile ) )
            {
                mEofPos = ::ftell( mTrackerFile );
                
                error = 0;
            }
        }
    
    }
    
    return error;
    
}

bool BCasterTracker::IsOpen()
{
  
    if ( mTrackerFile == NULL )
        return false;
     else
        return true;
    
    
}


BCasterTracker::BCasterTracker( const char* name )
{   

    mEofPos         = 0;
    mTrackerFile    = NULL;
    time_t calendarTime  = 0;
    
    calendarTime = ::time(NULL) + 10;

    while ( mTrackerFile == NULL && calendarTime > ::time(NULL) ) 
    {   mTrackerFile = ::fopen( name, "r+" );
        if ( !mTrackerFile )
        {   // try and create
            mTrackerFile = ::fopen( name, "a+" );

            if ( mTrackerFile )
            {
                (void)::chmod( name, S_IRUSR | S_IWUSR |  S_IRGRP | S_IWGRP |  S_IROTH | S_IWOTH );
                
                (void)::fclose(  mTrackerFile  );
            }
            
            mTrackerFile = NULL;
        }
        ::sleep(1);
    
    }
    ::strcpy(gTrackerFileTempDataPath,name);
    ::strcat( gTrackerFileTempDataPath, "_tmp" );
    FILE *  tempFile = NULL;
    calendarTime = ::time(NULL) + 10;
    while ( tempFile == NULL && calendarTime > ::time(NULL) ) 
    {   tempFile = ::fopen( gTrackerFileTempDataPath, "r+" );
        if ( !tempFile )
        {   // try and create
            tempFile = ::fopen( gTrackerFileTempDataPath, "a+" );
            if ( tempFile )
            {
                (void)::chmod( gTrackerFileTempDataPath, S_IRUSR | S_IWUSR |  S_IRGRP | S_IWGRP |  S_IROTH | S_IWOTH );
            }
        }
    
    }   
    if (tempFile) 
        (void)::fclose(  tempFile  );

    if ( mTrackerFile )
    {
        bool    haveLock = false;
        
        while ( !haveLock && calendarTime > ::time(NULL) ) 
        {
            struct flock    lock_params;
            
            lock_params.l_start = 0;    /* starting offset */
            lock_params.l_len = 0;      /* len = 0 means until end of file */
            lock_params.l_pid = getpid();       /* lock owner */
            lock_params.l_type = F_WRLCK;       /* lock type: read/write, etc. */
            lock_params.l_whence = 0;   /* type of l_start */
            
            if ( !::fcntl( fileno(mTrackerFile), F_SETLK, &lock_params ) )
                haveLock = true;
                
        }
    
        // if can't lock it, close it.
        if ( !haveLock )
        {   (void)::fclose(  mTrackerFile  );
            mTrackerFile = NULL;
        }

    }
        
    if ( mTrackerFile )
    {
        long    lineBuffSize = kTrackerLineBuffSize;        
        long    wordBuffSize = kTrackerLineBuffSize;        
        char    lineBuff[kTrackerLineBuffSize];
        char    wordBuff[kTrackerLineBuffSize];
        
        char    *next;
        UInt32  pid;
        int     error = 0;
        
        error = ::fseek( mTrackerFile, 0, SEEK_SET );
        Assert(error == 0);
        do 
        {   
            
            if ( ::fgets( lineBuff, lineBuffSize, mTrackerFile ) == NULL )
                break;
            
            // trim the leading whitespace
            next = ::TrimLeft( lineBuff );
            
            if (*next)
            {
                // grab the pid
                next = ::GetWord( wordBuff, next, wordBuffSize );
                        
                Assert( *wordBuff );
                
                pid = ::atoi( wordBuff );
                
                if (*next)
                    next = ::TrimLeft( next );
                // grab the broadcast list string

                if (*next)
                {
                    next = ::TrimLeft( next );
                    
                    if (*next == '"' )
                        next = ::GetQuotedWord( wordBuff, next, wordBuffSize );
                    else
                        next = ::GetWord( wordBuff, next, wordBuffSize );
                    
                    if ( this->IsProcessRunning( pid ) )
                    {
                        if (*wordBuff)
                        {   error = this->Add( pid, wordBuff );     
                                    
                        }
                    }
                }
                
            }
        
        } while ( feof( mTrackerFile ) == 0 && error == 0 );
    
        
        mEofPos = ::ftell( mTrackerFile );
    }
}
