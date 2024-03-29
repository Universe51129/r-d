#ifndef kVersionString
#include "revision.h"
#endif


#include "BroadcastLog.h"

static Bool16 sLogTimeInGMT = false;

static char* sLogHeader =   "#Software: %s\n"
                            "#Version: %s\n"    //%s == version
                            "#Date: %s\n"       //%s == date/time
                            "#Remark: All date values are in %s.\n" //%s == "GMT" or "local time"
                            "#Fields: date time filepath title copyright comment author artist album duration result\n";


BroadcastLog::BroadcastLog( PLBroadcastDef* broadcastParms, StrPtrLen* defaultPathPtr ) 
            : QTSSRollingLog() 
{
    *mDirPath = 0;
    *mLogFileName = 0;
    mWantsLogging = false;
    
    if (broadcastParms->mLogging)
    {
        if (!::strcmp( broadcastParms->mLogging, "enabled" ) )
        {
            mWantsLogging = true;
            
            ::strcpy( mDirPath, broadcastParms->mLogFile );
            char*   nameBegins = ::strrchr( mDirPath, kPathDelimiterChar );
            if ( nameBegins )
            {
                *nameBegins = 0; // terminate mDirPath at the last PathDelimeter
                nameBegins++;
                ::strcpy( mLogFileName, nameBegins );
            }
            else
            {   // it was just a file name, no dir spec'd
                memcpy(mDirPath,defaultPathPtr->Ptr,defaultPathPtr->Len);
                mDirPath[defaultPathPtr->Len] = 0;
                
                ::strcpy( mLogFileName, broadcastParms->mLogFile );
            }
            
        }
    }
    
    this->SetLoggingEnabled(mWantsLogging);
}

time_t BroadcastLog::WriteLogHeader(FILE *inFile)
{
    time_t calendarTime = ::time(NULL);
    Assert(-1 != calendarTime);
    if (-1 == calendarTime)
        return -1;

    struct tm  timeResult;
    struct tm* theLocalTime = qtss_localtime(&calendarTime, &timeResult);
    Assert(NULL != theLocalTime);
    if (NULL == theLocalTime)
        return -1;
    
    char tempBuffer[1024] = { 0 };
    qtss_strftime(tempBuffer, sizeof(tempBuffer), "#Log File Created On: %m/%d/%Y %H:%M:%S\n", theLocalTime);
    this->WriteToLog(tempBuffer, !kAllowLogToRoll);
    tempBuffer[0] = '\0';

    char theDateBuffer[QTSSRollingLog::kMaxDateBufferSizeInBytes] = { 0 };
    Bool16 result = QTSSRollingLog::FormatDate(theDateBuffer, false);
    
    if (result)
    {
        qtss_sprintf(tempBuffer, sLogHeader, "PlaylistBroadcaster" , kVersionString, 
                            theDateBuffer, sLogTimeInGMT ? "GMT" : "local time");
        this->WriteToLog(tempBuffer, !kAllowLogToRoll);
    }
        
    return calendarTime;
}


void    BroadcastLog::LogInfo( const char* infoStr )
{
    char    strBuff[1024] = "# ";
    char    dateBuff[80] = "";
    
    if ( this->FormatDate( dateBuff, false ) )
    {   
        if  (   (NULL != infoStr) 
            &&  ( ( strlen(infoStr) + strlen(strBuff) + strlen(dateBuff)  ) < 800)
            )
        {
            qtss_sprintf(strBuff,"#Remark: %s %s\n",dateBuff, infoStr);
            this->WriteToLog( strBuff, kAllowLogToRoll );
        }
        else
        {   
            ::strcat(strBuff,dateBuff);
            ::strcat(strBuff," internal error in LogInfo\n");
            this->WriteToLog( strBuff, kAllowLogToRoll );       
        }

    }
    
}


void BroadcastLog::LogMediaError( const char* path, const char* errStr , const char* messageStr)
{
    // log movie play info
    char    strBuff[1024] = "";
    char    dateBuff[80] = "";
    
    if ( this->FormatDate( dateBuff, false ) )
    {   
        if  (   (NULL != path) 
                &&  ( (strlen(path) + strlen(dateBuff) ) < 800)
            )
        {

            qtss_sprintf(strBuff,"#Remark: %s %s ",dateBuff, path);
                    
            if ( errStr )
            {   if  ( (strlen(strBuff) + strlen(errStr) ) < 1000 )
                {
                    ::strcat(strBuff,"Error:");
                    ::strcat(strBuff,errStr);
                }
            }
            else
                if  (   (NULL != messageStr) 
                        && 
                        ( (strlen(strBuff) + strlen(messageStr) ) < 1000 )
                    )
                {   ::strcat(strBuff,messageStr);
                }
                else
                    ::strcat(strBuff,"OK");
                
            ::strcat(strBuff,"\n");
            this->WriteToLog(strBuff, kAllowLogToRoll );
        }
        else
        {   
            ::strcat(strBuff,dateBuff);
            ::strcat(strBuff," internal error in LogMediaError\n");
            this->WriteToLog( strBuff, kAllowLogToRoll );       
        }

    }
    
}

void BroadcastLog::LogMediaData(	const char* path, const char* title, const char* copyright, 
                                    const char* comment, const char* author, const char* artist, 
                                    const char* album, UInt32 duration, SInt16 result)
{
    // log movie play info
    char    strBuff[1024] = "";
    char    dateBuff[80] = "";
    
    if ( this->FormatDate( dateBuff, false ) )
    {   
        if  (   (NULL != path) 
                &&  ( (strlen(path) + strlen(dateBuff) ) < 800)
            )
        {

            qtss_sprintf(strBuff,"%s '%s'",dateBuff, path);
                    
            if ( title || title[0] != 0)
            {   if  ( (strlen(strBuff) + strlen(title) ) < 1000 )
                {
                    ::strcat(strBuff," '");
                    ::strcat(strBuff, title);
                    ::strcat(strBuff,"'");
               }
            }
            else
            {
                ::strcat(strBuff," -");
            }
               
            if ( copyright || copyright[0] != 0)
            {   if  ( (strlen(strBuff) + strlen(copyright) ) < 1000 )
                {
                    ::strcat(strBuff," '");
                    ::strcat(strBuff, copyright);
                    ::strcat(strBuff,"'");
               }
            }
            else
            {
                ::strcat(strBuff," -");
            }
               
            if ( comment || comment[0] != 0)
            {   if  ( (strlen(strBuff) + strlen(comment) ) < 1000 )
                {
                    ::strcat(strBuff," '");
                    ::strcat(strBuff, comment);
                    ::strcat(strBuff,"'");
               }
            }
            else
            {
                ::strcat(strBuff," -");
            }
               
            if ( author || author[0] != 0)
            {   if  ( (strlen(strBuff) + strlen(author) ) < 1000 )
                {
                    ::strcat(strBuff," '");
                    ::strcat(strBuff, author);
                    ::strcat(strBuff,"'");
               }
            }
            else
            {
                ::strcat(strBuff," -");
            }
               
            if ( artist || artist[0] != 0)
            {   if  ( (strlen(strBuff) + strlen(artist) ) < 1000 )
                {
                    ::strcat(strBuff," '");
                    ::strcat(strBuff, artist);
                    ::strcat(strBuff,"'");
               }
            }
            else
            {
                ::strcat(strBuff," -");
            }
               
            if ( album || album[0] != 0)
            {   if  ( (strlen(strBuff) + strlen(album) ) < 1000 )
                {
                    ::strcat(strBuff," '");
                    ::strcat(strBuff, album);
                    ::strcat(strBuff,"'");
               }
            }
            else
            {
                ::strcat(strBuff," -");
            }
               
            // add the duration in seconds
            qtss_sprintf(dateBuff, " %ld ", duration);
            ::strcat(strBuff,dateBuff);
            
            // add the result code
            qtss_sprintf(dateBuff, " %d\n", result);
            ::strcat(strBuff,dateBuff);
            
            this->WriteToLog(strBuff, kAllowLogToRoll );
        }
        else
        {   
            ::strcat(strBuff,dateBuff);
            ::strcat(strBuff," internal error in LogMediaData\n");
            this->WriteToLog( strBuff, kAllowLogToRoll );       
        }

    }
    
}


