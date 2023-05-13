#ifndef __PLBroadcastDef__
#define __PLBroadcastDef__
#include "OSHeaders.h"
#include "BroadcasterSession.h"

class PLBroadcastDef {

    public:
        PLBroadcastDef( const char* setupFileName,  char *destinationIP, Bool16 debug );
        virtual ~PLBroadcastDef();
        
        Bool16  ParamsAreValid() { return mParamsAreValid; }
        
        void    ValidateSettings();
        void    ShowErrorParams();
        
        void    ShowSettings();
        
        static Bool16 ConfigSetter( const char* paramName, const char* paramValue[], void * userData );

                                        // * == default value, <r> required input
        char*   mDestAddress;           // set by PLB to be resolved address
        char*   mOrigDestAddress;       // [0.0.0.0 | domain name?] *127.0.0.1 ( self )
        char*   mBasePort;              // [ 0...32k?] *5004
        
        
        char*   mPlayMode;              // [sequential | *sequential_looped | weighted]
        // removed at build 12 char*    mLimitPlay;             // [*enabled | disabled]
        int     mLimitPlayQueueLength; // [ 0...32k?] *20
        //char* mLimitPlayQueueLength; // [ 0...32k?] *20
        char*   mPlayListFile;          // [os file path] *<PLBroadcastDef-name>.ply
        char*   mSDPFile;               // [os file path] <r>
        char*   mLogging;               // [*enabled | disabled]
        char*   mLogFile;               // [os file path] *<PLBroadcastDef-name>.log
        char*   mSDPReferenceMovie;     // [os file path]
        char*   mCurrentFile;           // [os file path] *<PLBroadcastDef-name>.current
        char*   mUpcomingFile;          // [os file path] *<PLBroadcastDef-name>.upcoming
        char*   mReplaceFile;           // [os file path] *<PLBroadcastDef-name>.replacelist
        char*   mStopFile;              // [os file path] *<PLBroadcastDef-name>.stoplist
        char*   mInsertFile;            // [os file path] *<PLBroadcastDef-name>.insertlist
        char*   mShowCurrent;           // [*enabled | disabled]
        char*   mShowUpcoming;          // [*enabled | disabled]
        
        BroadcasterSession *mTheSession;// a broadcaster RTSP/RTP session with the server.
        
        bool    mIgnoreFileIP;
        char*   mMaxUpcomingMovieListSize; // [ 2^31] *7
        char*   mDestSDPFile;           // [movies folder relative file path] 
        
        char*   mStartTime;         // NTP start time
        char*   mEndTime;           // NTP end time
        char*   mIsDynamic;         // true
        
        char*   mName;              // Authentication name
        char*   mPassword;          // Authentication password
        
        char *  mTTL;               // TTL for multicast [1..15] *1
        
        char *  mRTSPPort;

        char *  mPIDFile;
        
        char *  mClientBufferDelay;     
        
        enum {
            kInvalidNone = 0x00000000,
            kInvalidDestAddress = 0x00000001
        };
        
    protected:
        Bool16  mParamsAreValid;
        UInt32  mInvalidParamFlags;
        Bool16  SetValue( char** dest, const char* value);
        Bool16  SetDefaults( const char* setupFileName );
        Bool16  fDebug;
};

#endif
