#ifndef __BroadcastLog__
#define __BroadcastLog__

#include "QTSSRollingLog.h"
#include "PLBroadcastDef.h"
#include "StrPtrLen.h"
#include <string.h>

class BroadcastLog : public QTSSRollingLog
{
    enum { eLogMaxBytes = 0, eLogMaxDays = 0 };
    
    public:
        BroadcastLog( PLBroadcastDef* broadcastParms, StrPtrLen* defaultPathPtr);
        virtual ~BroadcastLog() {}
    
        virtual char* GetLogName() 
        {   // RTSPRollingLog wants to see a "new'd" copy of the file name
            char*   name = new char[strlen( mLogFileName ) + 1 ];
            
            if ( name )
                ::strcpy( name, mLogFileName );

            return name;
        }       
        
        virtual char* GetLogDir() 
        {   // RTSPRollingLog wants to see a "new'd" copy of the file name
            char *name = new char[strlen( mDirPath ) + 1 ];
            
            if ( name )
                ::strcpy( name, mDirPath );

            return name;
        }       
                                                                                
        virtual UInt32 GetRollIntervalInDays() { return eLogMaxDays; /* we dont' roll*/ }
                                            
        virtual UInt32 GetMaxLogBytes() {  return eLogMaxBytes; /* we dont' roll*/ }
        
        void    LogInfo( const char* infoStr );
        void    LogMediaError( const char* path, const char* errStr, const char* messageStr);
        
        void    LogMediaData(	const char* path, const char* title, const char* copyright, 
                                const char* comment, const char* author, const char* artist, 
                                const char* album, UInt32 duration, SInt16 result);

        bool    WantsLogging() { return mWantsLogging; }
        const char* LogFileName() { return mLogFileName; }
        const char* LogDirName() { return mDirPath; }
        
        virtual time_t  WriteLogHeader(FILE *inFile);
        
    protected:
        char    mDirPath[256];
        char    mLogFileName[256];
        bool    mWantsLogging;
    
};

#endif
