#ifndef __FILEPREFSSOURCE_H__
#define __FILEPREFSSOURCE_H__

#include "PrefsSource.h"
#include "OSHeaders.h"

class KeyValuePair; //only used in the implementation

class FilePrefsSource : public PrefsSource
{
    public:
    
        FilePrefsSource( Bool16 allowDuplicates = false );
        virtual ~FilePrefsSource(); 
    
        virtual int     GetValue(const char* inKey, char* ioValue);
        virtual int     GetValueByIndex(const char* inKey, UInt32 inIndex, char* ioValue);

        char*           GetValueAtIndex(UInt32 inIndex);
        char*           GetKeyAtIndex(UInt32 inIndex);
        UInt32          GetNumKeys() { return fNumKeys; }
        
        int InitFromConfigFile(const char* configFilePath);
        void WriteToConfigFile(const char* configFilePath);

        void SetValue(const char* inKey, const char* inValue);
        void DeleteValue(const char* inKey);

    private:
    
        static Bool16 FilePrefsConfigSetter( const char* paramName, const char* paramValue[], void* userData );
        
        KeyValuePair*   FindValue(const char* inKey, char* ioValue, UInt32 index = 0);
        KeyValuePair*   fKeyValueList;
        UInt32          fNumKeys;
        Bool16 fAllowDuplicates;
};

#endif //__FILEPREFSSOURCE_H__
