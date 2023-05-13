#ifndef playlist_parsers_H
#define playlist_parsers_H

#include <stdio.h>
#include <stdlib.h>
#include "SafeStdLib.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "playlist_elements.h"
#include "OSHeaders.h"
#include "playlist_SimpleParse.h"
#include "playlist_utils.h"
#include "playlist_lists.h"

#ifndef __Win32__
    #include <sys/types.h>
#endif


class TextLine : public SimpleParser 
{
    public: 
        ArrayList<SimpleString> fWords;
        int Parse (SimpleString *textStrPtr);
};



class LineAndWordsParser : public SimpleParser 
{
    public: 
        ArrayList<TextLine> fLines;
        int Parse (SimpleString *textStrPtr);
};


class SDPFileParser : SimpleParser
{
    public:
                SDPFileParser(void) : fNumTracks(0), fSDPBuff(NULL)  {}
                ~SDPFileParser(void);
    SInt32      ReadSDP(char *theFile);
    SInt32      ParseSDP(char *theBuff) ;
    SInt32      GetNumTracks() {return fNumTracks;} ;
    bool        IsCommented(SimpleString *aLine);
    TextLine*   GetRTPMapLines(TextLine *theLinePtr,TypeMap *theTypeMapPtr);
    bool        GetRTPMap(TextLine *theLinePtr,PayLoad *payloadPtr);
    bool        GetMediaFromLine(TextLine *theLinePtr, TypeMap *theTypeMapPtr);
    void        GetPayLoadsFromLine(TextLine *theLinePtr, TypeMap *theTypeMapPtr);
    TextLine*   GetTrackID(TextLine *theLinePtr,TypeMap *theTypeMapPtr);
    bool        ParseIPString(TextLine *theLinePtr);
    SimpleString* GetIPString() { return &fIPAddressString; };
    
    ArrayList<SimpleString> fQTTextLines;
    SInt32      GetNumQTTextLines() {return fNumQTTextLines;} ;
    bool        GetQTTextFromLine(TextLine *theLinePtr);
    
    SDPMediaList fSDPMediaList;
    protected:
    
    UInt32  TimeScaleLookUp(int payLoadID, SimpleString *typeStringPtr);
    short   CountMediaEntries() ;
    short   CountRTPMapEntries() ;
    short   CountQTTextLines();
    
    UInt32  fNumQTTextLines;
    UInt32  fNumTracks;
    LineAndWordsParser  fParser;
    SimpleString        fIPAddressString;

    private: 
    enum { cMaxBytes = 4096}; // maximum accepted sdp file size
    
    static char* sMediaTag;
    static char* sAttributeTag;
    static char* sConnectionTag;

        char *fSDPBuff;
};


#endif //playlist_parsers_H
