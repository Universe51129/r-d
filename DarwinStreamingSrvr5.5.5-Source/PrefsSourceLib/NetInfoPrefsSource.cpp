#include "nilib2.h"

#include "NetInfoPrefsSource.h"

static char* gQTSSPropertiesPath = "/services/QuickTimeStreamingServer";

NetInfoPrefsSource::NetInfoPrefsSource()
{}

int NetInfoPrefsSource::GetValue(const char* inKey, char* ioValue)
{
    return this->GetValueByIndex(inKey, 0, ioValue);
}

int NetInfoPrefsSource::GetValueByIndex(const char* inKey, UInt32 inIndex, char* ioValue)
{
    ni_status status = NI_OK;
    ni_namelist nameList = {};
    void* localDomain = NULL;
    ni_id qtssDir = {}; 
    
    ioValue[0] = '\0';

    status = ni_open(NULL, ".", &localDomain);
    if (status != NI_OK)
        return false;
        
    if (status == NI_OK)
        status = ni_pathsearch(localDomain, &qtssDir, gQTSSPropertiesPath);
        
    if (status == NI_OK)
        status = ni_lookupprop(localDomain, &qtssDir, inKey, &nameList);

    if (status == NI_OK)
    {
        if (nameList.ni_namelist_len > inIndex)
            strcpy(ioValue, nameList.ni_namelist_val[inIndex]);
        else
            status = NI_BADID;
        ni_namelist_free(&nameList);
    }
    
    ni_free(localDomain);
    
    return (status == NI_OK);
}

void NetInfoPrefsSource::SetValue(char* inKey, char* inValue)
{
    this->SetValueByIndex(inKey, inValue, NI_INDEX_NULL);
}

void NetInfoPrefsSource::SetValueByIndex(char* inKey, char* inValue, UInt32 inIndex)
{
    void* localDomain = NULL;
    ni_status status = NI_OK;
    ni_namelist nameList = {};

    ni_namelist_insert(&nameList, inValue, inIndex);

    status = ni_open(NULL, ".", &localDomain);

    if (status == NI_OK)
    {
        status = ni2_create(localDomain, gQTSSPropertiesPath);

        if (status == NI_OK)
            status = ni2_appendprop(localDomain, gQTSSPropertiesPath, inKey, nameList);
    }
    ni_namelist_free(&nameList);
    ni_free(localDomain);
}
