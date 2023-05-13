#ifndef __XML_PREFS_PARSER__
#define __XML_PREFS_PARSER__

#include "OSFileSource.h"
#include "OSQueue.h"
#include "StringParser.h"
#include "XMLParser.h"

typedef XMLTag* ContainerRef;

class XMLPrefsParser : public XMLParser
{
    public:
    
        XMLPrefsParser(char* inPath);
        ~XMLPrefsParser();
    
       
        int     Parse();

        int     WritePrefsFile();

        ContainerRef    GetRefForModule( char* inModuleName, Bool16 create = true);
        
        ContainerRef    GetRefForServer();
        
        UInt32  GetNumPrefValues(ContainerRef pref);
        
        UInt32  GetNumPrefsByContainer(ContainerRef container);
        
        char*   GetPrefValueByIndex(ContainerRef container, const UInt32 inPrefsIndex, const UInt32 inValueIndex,
                                            char** outPrefName, char** outDataType);
                                        
        char*   GetPrefValueByRef(ContainerRef pref, const UInt32 inValueIndex,
                                            char** outPrefName, char** outDataType);
                                        
        ContainerRef    GetObjectValue(ContainerRef pref, const UInt32 inValueIndex);

        ContainerRef    GetPrefRefByName(   ContainerRef container,
                                            const char* inPrefName);
        
        ContainerRef    GetPrefRefByIndex(  ContainerRef container,
                                            const UInt32 inPrefsIndex);
        
        ContainerRef    AddPref( ContainerRef container, char* inPrefName, char* inPrefDataType );

        void    ChangePrefType( ContainerRef pref, char* inNewPrefDataType);
                            
        void    AddNewObject( ContainerRef pref );

        void    AddPrefValue(   ContainerRef pref, char* inNewValue);
        
        void    SetPrefValue(   ContainerRef pref, const UInt32 inValueIndex,
                                char* inNewValue);
        
        void    RemovePrefValue(    ContainerRef pref, const UInt32 inValueIndex);

        void    RemovePref( ContainerRef pref );
                
    private:
        
        XMLTag*     GetConfigurationTag();
};

#endif //__XML_PREFS_PARSER__
