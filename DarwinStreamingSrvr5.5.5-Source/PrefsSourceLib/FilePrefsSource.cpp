#include "FilePrefsSource.h"
#include <string.h>
#include <stdio.h>

#include <errno.h>

#include "MyAssert.h"
#include "OSMemory.h"
#include "ConfParser.h"

const int kMaxLineLen = 2048;
const int kMaxValLen = 1024;

class KeyValuePair
{
 public:
 
    char*   GetValue() { return fValue; }
    
 private:
    friend class FilePrefsSource;

    KeyValuePair(const char* inKey, const char* inValue, KeyValuePair* inNext);
    ~KeyValuePair();

    char* fKey;
    char* fValue;
    KeyValuePair* fNext;

     void ResetValue(const char* inValue);
};


KeyValuePair::KeyValuePair(const char* inKey, const char* inValue, KeyValuePair* inNext) :
    fKey(NULL),
    fValue(NULL),
    fNext(NULL)
{
    fKey = NEW char[::strlen(inKey)+1];
    ::strcpy(fKey, inKey);
    fValue = NEW char[::strlen(inValue)+1];
    ::strcpy(fValue, inValue);
    fNext = inNext;
}


KeyValuePair::~KeyValuePair()
{
    delete [] fKey;
    delete [] fValue;
}


void KeyValuePair::ResetValue(const char* inValue)
{
    delete [] fValue;
    fValue = NEW char[::strlen(inValue)+1];
    ::strcpy(fValue, inValue);
}


FilePrefsSource::FilePrefsSource( Bool16 allowDuplicates)
:   fKeyValueList(NULL),
    fNumKeys(0),
    fAllowDuplicates(allowDuplicates)
{
    
}

FilePrefsSource::~FilePrefsSource()
{
    while (fKeyValueList != NULL)
    {
        KeyValuePair* keyValue = fKeyValueList;
        fKeyValueList = fKeyValueList->fNext;
        delete keyValue;
    }

}

int FilePrefsSource::GetValue(const char* inKey, char* ioValue)
{
    return (this->FindValue(inKey, ioValue) != NULL);
}


int FilePrefsSource::GetValueByIndex(const char* inKey, UInt32 inIndex, char* ioValue)
{
    KeyValuePair* thePair = this->FindValue(inKey, ioValue, inIndex);
    
    if (thePair == NULL)
        return false;
    
    return true;
   }

char* FilePrefsSource::GetValueAtIndex(UInt32 inIndex)
{
    KeyValuePair* thePair = fKeyValueList;
    while ((thePair != NULL) && (inIndex-- > 0))
        thePair = thePair->fNext;
        
    if (thePair != NULL)
        return thePair->fValue;
    return NULL;
}

char* FilePrefsSource::GetKeyAtIndex(UInt32 inIndex)
{
    KeyValuePair* thePair = fKeyValueList;
    while ((thePair != NULL) && (inIndex-- > 0))
        thePair = thePair->fNext;
        
    if (thePair != NULL)
        return thePair->fKey;
    return NULL;
}

void FilePrefsSource::SetValue(const char* inKey, const char* inValue)
{
    KeyValuePair* keyValue = NULL;

    if ((!fAllowDuplicates) && ((keyValue = this->FindValue(inKey, NULL)) != NULL))
    {
        keyValue->ResetValue(inValue);
    }
    else
    {
        fKeyValueList  = NEW KeyValuePair(inKey, inValue, fKeyValueList);
        fNumKeys++;
    }
}



Bool16 FilePrefsSource::FilePrefsConfigSetter( const char* paramName, const char* paramValue[], void* userData )
{
    int     valueIndex = 0;
    
    FilePrefsSource *theFilePrefs = (FilePrefsSource*)userData;
    
    Assert( theFilePrefs );
    Assert(  paramName );

    while ( paramValue[valueIndex] != NULL )
    {   
        theFilePrefs->SetValue(paramName, paramValue[valueIndex] );
        valueIndex++;
    }
    
    return false; // always succeeds
}


int FilePrefsSource::InitFromConfigFile(const char* configFilePath)
{
    
    return ::ParseConfigFile( true, configFilePath, FilePrefsConfigSetter, this );
    
}

void FilePrefsSource::DeleteValue(const char* inKey)
{
    KeyValuePair* keyValue = fKeyValueList;
    KeyValuePair* prevKeyValue = NULL;
    
    while (keyValue != NULL)
    {
        if (::strcmp(inKey, keyValue->fKey) == 0)
        {
            if (prevKeyValue != NULL)
            {
                prevKeyValue->fNext = keyValue->fNext;
                delete keyValue;
            }
            else
            {
                fKeyValueList = prevKeyValue;
            }
            
            return;
            
        }
        prevKeyValue = keyValue;
        keyValue = keyValue->fNext;
    }
}


void FilePrefsSource::WriteToConfigFile(const char* configFilePath)
{
    int err = 0;
    FILE* fileDesc = ::fopen( configFilePath,   "w");

        if (fileDesc != NULL)
        {
            err = ::fseek(fileDesc, 0, SEEK_END);
            Assert(err == 0);

            KeyValuePair* keyValue = fKeyValueList;

            while (keyValue != NULL)
            {
                    (void)qtss_fprintf(fileDesc, "%s   %s\n\n", keyValue->fKey, keyValue->fValue);

                    keyValue = keyValue->fNext;
            }

            err = ::fclose(fileDesc);
            Assert(err == 0);
        }
}


KeyValuePair* FilePrefsSource::FindValue(const char* inKey, char* ioValue, UInt32 index )
{
    KeyValuePair    *keyValue = fKeyValueList;
    UInt32          foundIndex = 0;

        if ( ioValue != NULL)
            ioValue[0] = '\0';
    
    while (keyValue != NULL)
    {
        if (::strcmp(inKey, keyValue->fKey) == 0)
        {
            if ( foundIndex == index )
            {
                if (ioValue != NULL)
                    ::strcpy(ioValue, keyValue->fValue);
                return keyValue;
            }
            foundIndex++;
        }
        keyValue = keyValue->fNext;
    }
    
    return NULL;
}
