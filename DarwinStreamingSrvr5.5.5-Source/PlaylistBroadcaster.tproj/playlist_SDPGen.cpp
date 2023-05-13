#include "playlist_utils.h"
#include "playlist_SDPGen.h"
#include "playlist_broadcaster.h"
#include "QTRTPFile.h"
#include "OS.h"
#include "SocketUtils.h"
#include "SDPUtils.h"
#include "OSArrayObjectDeleter.h"

short SDPGen::AddToBuff(char *aSDPFile, short currentFilePos, char *chars)
{
    short charLen = strlen(chars);
    short newPos = currentFilePos + charLen;
    
    if (newPos <= eMaxSDPFileSize)
    {   memcpy(&aSDPFile[currentFilePos],chars,charLen); // only the chars not the \0
        aSDPFile[currentFilePos +charLen] = '\0';
    }   
    else
    {   newPos =  (-newPos);    
    }
    currentFilePos = newPos;
    
    return currentFilePos;
}

UInt32 SDPGen::RandomTime(void)
{
    SInt64 curTime = 0;
	curTime = PlayListUtils::Milliseconds();

    curTime += rand() ;
    return (UInt32) curTime;
}

short  SDPGen::GetNoExtensionFileName(char *pathName, char *result, short maxResult)
{
    char *start;
    char *end;
    char *sdpPath = pathName;
    short pathNameLen = strlen(pathName);
    short copyLen = 0;
    
    
    do 
    {
        start = strrchr(sdpPath, ePath_Separator);
        if(start  == NULL ) // no path separator
        {   start = sdpPath;
            copyLen =   pathNameLen;
            break;
        } 
        
        start ++; // move start to one past the separator
        end = strrchr(sdpPath, eExtension_Separator);       
        if (end == NULL) // no extension
        {   copyLen = strlen(start) + 1;
            break;
        }
        
        // both path separator and an extension
        short startLen = strlen(start);
        short endLen = strlen(end);
        copyLen = startLen - endLen;
        
    } while (false);
    
    if (copyLen > maxResult)
        copyLen = maxResult;
        
    memcpy(result, start, copyLen); 
    result[copyLen] = '\0';
    
    return copyLen;
}
            


char *SDPGen::Process(  char *sdpFileName,
                        char * basePort,
                        char *ipAddress,
                        char *anSDPBuffer,
                        char *startTime,
                        char *endTime,  
                        char *isDynamic,
                        char *ttl,
                        int *error
                        )
{
    char *resultBuf = NULL;
    short currentPos = 0;
    short trackID = 1;
    *error = -1;
    do
    {   
        fSDPFileContentsBuf = new char[eMaxSDPFileSize];
        {   char version[] = "v=0\r\n";
        
            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, version);
            if (currentPos < 0) break;
        }
            
        {   char *userName = "QTSS_Play_List";
            UInt32 sessIDAsRandomTime = RandomTime();
            UInt32 versAsRandomTime = RandomTime();
            char  ownerLine[255];

            Assert(SocketUtils::GetIPAddrStr(0) != NULL);
            Assert(SocketUtils::GetIPAddrStr(0)->Ptr != NULL);
            qtss_sprintf(ownerLine, "o=%s %lu %lu IN IP4 %s\r\n",userName ,sessIDAsRandomTime,versAsRandomTime,SocketUtils::GetIPAddrStr(0)->Ptr);
            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, ownerLine);
            if (currentPos < 0) break;
        }

        {   enum { eMaxSessionName = 64};
            char newSessionName[eMaxSessionName];
            short nameSize = 0;
            char  sessionLine[255];
            nameSize = GetNoExtensionFileName(sdpFileName, newSessionName, eMaxSessionName);
            if (nameSize < 0) break;
        
            qtss_sprintf(sessionLine, "s=%s\r\n", newSessionName);
            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, sessionLine);
            if (currentPos < 0) break;
        }
        
        //    c=IN IP4 (destinatin ip address)
        {   
            char  sdpLine[255];
            if (SocketUtils::IsMulticastIPAddr(ntohl(inet_addr(ipAddress))))
                qtss_sprintf(sdpLine, "c=IN IP4 %s/%s\r\n", ipAddress,ttl);
            else
                 qtss_sprintf(sdpLine, "c=IN IP4 %s\r\n", ipAddress);
           currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, sdpLine);
            if (currentPos < 0) break;
        }
        
        {
            char  controlLine[255];
            if ( 0 == ::strcmp( "enabled", isDynamic) )
                qtss_sprintf(controlLine, "a=x-broadcastcontrol:RTSP\r\n");          
            else
                qtss_sprintf(controlLine, "a=x-broadcastcontrol:TIME\r\n");          
            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, controlLine);
            if (currentPos < 0) break;
        }
                
        
        {
            char  timeLine[255];
            UInt32 startTimeNTPSecs = strtoul(startTime, NULL, 10);
            UInt32 endTimeNTPSecs = strtoul(endTime, NULL, 10);
            qtss_sprintf(timeLine, "t=%lu %lu\r\n", startTimeNTPSecs, endTimeNTPSecs);           
            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, timeLine);
            if (currentPos < 0) break;
        }

        {
            SimpleString resultString;
            SimpleString sdpBuffString(anSDPBuffer);
    
            short numLines = 0;
            char *found = NULL;
            
            enum  {eMaxLineLen = 1024};
            char aLine[eMaxLineLen];
            ::memset(aLine, 0, eMaxLineLen);
            
            int portCount = atoi(basePort);
            
            
            
            SimpleParser sdpParser;
            while ( sdpParser.GetLine(&sdpBuffString,&resultString) ) 
            {                           
                numLines ++;
                if (resultString.fLen > 1024) continue;
                 
                memcpy(aLine,resultString.fTheString,resultString.fLen);
                aLine[resultString.fLen] = '\0';
                
                int newBuffSize = sdpBuffString.fLen - (resultString.fLen);
                char *newBuffPtr = &resultString.fTheString[resultString.fLen];
                
                sdpBuffString.SetString(newBuffPtr, newBuffSize);   

                char firstChar = aLine[0];
                {                   
                    if (firstChar == 'v') continue;// (protocol version)
                    if (firstChar == 'o') continue; //(owner/creator and session identifier).
                    if (firstChar == 's') continue; //(session name)
                    if (firstChar == 'c') continue; //(connection information - optional if included at session-level)
                }
                
                {   
                    if (firstChar == 't') continue;// (time the session is active)              
                    if (firstChar == 'r') continue;// (zero or more repeat times)

                    found = strstr(aLine, "a=control:trackID"); // turn this off
                    if (!fKeepTracks)
                    {   
                        if (fAddIndexTracks)
                        {
                            if (found != NULL) 
                            {   char mediaLine[eMaxLineLen];                                            
                                qtss_sprintf(mediaLine,"a=control:trackID=%d\r\n",trackID);
                                currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, mediaLine); // copy rest of line starting with the transport protocol
                                trackID ++;
                            }
                        }
                        if (found != NULL) continue;
                    }
                    
                    
                    found = strstr(aLine,  "a=range"); // turn this off
                    if (found != NULL) continue;
                }
                
                { 
                    found = strstr(aLine,"m=");  //(media name and transport address)
                    if (found != NULL)
                    {   
                        char *startToPortVal = strtok(aLine," ");
                        strtok(NULL," "); 
                        if (found != NULL) 
                        {   char mediaLine[eMaxLineLen];                
                            char *protocol = strtok(NULL,"\r\n"); 
                            
                            qtss_sprintf(mediaLine,"%s %d %s\r\n",startToPortVal,portCount,protocol);
                            currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, mediaLine); 
                            if (portCount != 0)
                                portCount += 2; 
                            continue;
                        }
                    }
                }

                {   
                    short lineLen = strlen(aLine);
                    
                    // get rid of trailing characters
                    while (lineLen > 0 && (NULL == strchr("\r\n",aLine[lineLen]))  )
                    {   aLine[lineLen] = '\0';
                        lineLen --;
                    }
                    
                    // get rid of any line feeds and carriage returns
                    while (lineLen > 0 && (NULL != strchr("\r\n",aLine[lineLen])) )
                    {   aLine[lineLen] = '\0';
                        lineLen --;
                    }
                    aLine[lineLen + 1] = '\r';
                    aLine[lineLen + 2] = '\n';
                    aLine[lineLen + 3] = 0;
                    currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, aLine); // copy this line   
                }
            }

            //  buffer delay 
            if (fClientBufferDelay > 0.0)
            {   
                char  delayLine[255];
                qtss_sprintf(delayLine, "a=x-bufferdelay:%.2f\r\n", fClientBufferDelay);
                currentPos = AddToBuff(fSDPFileContentsBuf, currentPos, delayLine);
                if (currentPos < 0) break;
            }
        }
    

        StrPtrLen theSDPStr(fSDPFileContentsBuf);
        SDPContainer rawSDPContainer; 
        if (!rawSDPContainer.SetSDPBuffer( &theSDPStr ))
        {    delete [] fSDPFileContentsBuf;
             fSDPFileContentsBuf = NULL;
             return NULL; 
        }
    
        SDPLineSorter sortedSDP(&rawSDPContainer);
        resultBuf = sortedSDP.GetSortedSDPCopy(); // return a new copy of the sorted SDP
        delete [] fSDPFileContentsBuf; // this has to happen after GetSortedSDPCopy
        fSDPFileContentsBuf = resultBuf;
       
        *error = 0;
        
    } while (false);
        
    return resultBuf;
}

int SDPGen::Run(  char *movieFilename
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
               )
{
    int result = -1;
    int fdsdp = -1;
    bool sdpExists = false;
    
    do 
    {
        if (!movieFilename) break;
        if (!sdpFilename) break;
        
        if (buff && buffSize > 0) // set buff to 0 length string
            buff[0] = 0;
            
        fdsdp = open(sdpFilename, O_RDONLY, 0);     
        if (fdsdp != -1)
            sdpExists = true;
            
        if (sdpExists && !forceNewSDP)
        {   
            if (!overWriteSDP) 
            {
                if (buff && (buffSize > 0)) 
                {   int count = ::read(fdsdp,buff, buffSize -1);
                    if (count > 0) 
                        buff[count] = 0;
                }
                    
            }

            close(fdsdp);
            fdsdp = -1;
            
            if (!overWriteSDP) 
            {   result = 0;
                break; // leave nothing to do
            }
        }   
            
        QTRTPFile::ErrorCode err = fRTPFile.Initialize(movieFilename);
        result = QTFileBroadcaster::EvalErrorCode(err);
        if( result != QTRTPFile::errNoError ) 
            break;
                        
        // Get the file
        int     sdpFileLength=0;            
        int     processedSize=0;            
        char    *theSDPText = fRTPFile.GetSDPFile(&sdpFileLength);
        
        if( theSDPText == NULL || sdpFileLength <= 0) 
        {   break;
        }
               
        char *processedSDP = NULL;
        processedSDP = Process(sdpFilename, basePort, ipAddress, theSDPText,startTime,endTime,isDynamic, ttl, &result);
        if (result != 0) break;
        
        processedSize = strlen(processedSDP);
        
        if (buff != NULL)
        {   if (buffSize > processedSize )
            {    
                buffSize = processedSize;
            }
            memcpy(buff,processedSDP,buffSize);
            buff[buffSize] = 0;
        }
        
        if (!overWriteSDP && sdpExists) 
        {
            break;
        }
        // Create our SDP file and write out the data           
#ifdef __Win32__
        fdsdp = open(sdpFilename, O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, 0664);
#else
        fdsdp = open(sdpFilename, O_CREAT | O_TRUNC | O_WRONLY, 0664);
#endif
        if( fdsdp == -1 ) 
        {   
                        //result = -1;
                        result = -2;
                        break;
        }   
        write(fdsdp, processedSDP, processedSize);  
        result = 0;
        
        // report that we made a file
        fSDPFileCreated = true;
        
        
    } while (false);
    
    if (fdsdp != -1)
    {   result = 0;
        close(fdsdp);
        fdsdp = -1;
    }
    
    return result;

}

