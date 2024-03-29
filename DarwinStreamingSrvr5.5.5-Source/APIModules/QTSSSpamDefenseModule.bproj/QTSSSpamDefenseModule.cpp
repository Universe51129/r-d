/*
This code implements a Spam Defense module to prevent malicious users 
from consuming server resources by repeatedly establishing connections. 
The main functions implemented are as follows:

When the module is initialised, 
the num_conns_per_ip_addr parameter in the configuration file is read, 
indicating the number of connections allowed to be established per IP address.

When each client requests to establish a connection, 
it gets its IP address and checks if it has already established too many connections.
If the limit is exceeded, an error message is returned and the connection is rejected;
otherwise, the connection is established and the number of connections to that IP address is added by one.

When each client closes a connection, 
the number of connections to that IP address is subtracted by one. 
If no connection has been established to that IP address, it is not processed.

This Spam Defense module ensures efficient querying and modification 
by using a hash table to maintain the number of connections per IP address.
Also, to ensure data consistency during concurrency,
a mutually exclusive lock is used to protect the hash table.
*/
/*
    File:       QTSSSpamDefanseModule.cpp

    Contains:   Implementation of module described in .h file

    

*/

#include "QTSSSpamDefenseModule.h"
#include "OSHashTable.h"
#include "OSMutex.h"
#include "QTSSModuleUtils.h"
#include "OSMemory.h"

static QTSS_ModulePrefsObject sPrefs = NULL;

class IPAddrTableKey;

class IPAddrTableElem
{
    public:

        IPAddrTableElem(UInt32 inIPAddr) : fIPAddr(inIPAddr), fRefCount(0), fNextHashEntry(NULL) {}
        ~IPAddrTableElem() {}
        
        UInt32 GetRefCount() { return fRefCount; }
        void    IncrementRefCount() { fRefCount++; }
        void    DecrementRefCount() { fRefCount--; }
    private:
        
        UInt32              fIPAddr;// this also serves as the hash value
        UInt32              fRefCount;
                
        IPAddrTableElem*    fNextHashEntry;
        
        friend class IPAddrTableKey;
        friend class OSHashTable<IPAddrTableElem, IPAddrTableKey>;
};


class IPAddrTableKey
{
public:

    //CONSTRUCTOR / DESTRUCTOR:
    IPAddrTableKey(UInt32 inIPAddr) : fIPAddr(inIPAddr) {}
    ~IPAddrTableKey() {}
    
    
private:

    //PRIVATE ACCESSORS:    
    SInt32      GetHashKey()        { return fIPAddr; }

    //these functions are only used by the hash table itself. This constructor
    //will break the "Set" functions.
    IPAddrTableKey(IPAddrTableElem *elem) : fIPAddr(elem->fIPAddr) {}
                                    
    friend int operator ==(const IPAddrTableKey &key1, const IPAddrTableKey &key2)
    {
        return (key1.fIPAddr == key2.fIPAddr);
    }
    
    //data:
    UInt32  fIPAddr;

    friend class OSHashTable<IPAddrTableElem, IPAddrTableKey>;
};

typedef OSHashTable<IPAddrTableElem, IPAddrTableKey> IPAddrHashTable;

// STATIC DATA
static IPAddrHashTable*         sHashTable = NULL;
static OSMutex*                 sMutex;
static UInt32                   sNumConnsPerIP = 0;
static UInt32                   sDefaultNumConnsPerIP = 100;

// ATTRIBUTES
static QTSS_AttributeID         sIsFirstRequestAttr = qtssIllegalAttrID;
static QTSS_AttributeID         sTooManyConnectionsErr = qtssIllegalAttrID;

// FUNCTION PROTOTYPES
static QTSS_Error   QTSSSpamDefenseModuleDispatch(QTSS_Role inRole, QTSS_RoleParamPtr inParams);
static QTSS_Error   Register(QTSS_Register_Params* inParams);
static QTSS_Error Initialize(QTSS_Initialize_Params* inParams);
static QTSS_Error RereadPrefs();
static QTSS_Error Authorize(QTSS_StandardRTSP_Params* inParams);
static QTSS_Error SessionClosing(QTSS_RTSPSession_Params* inParams);

// FUNCTION IMPLEMENTATIONS


QTSS_Error QTSSSpamDefenseModule_Main(void* inPrivateArgs)
{
    return _stublibrary_main(inPrivateArgs, QTSSSpamDefenseModuleDispatch);
}


QTSS_Error  QTSSSpamDefenseModuleDispatch(QTSS_Role inRole, QTSS_RoleParamPtr inParams)
{
    switch (inRole)
    {
        case QTSS_Register_Role:
            return Register(&inParams->regParams);
        case QTSS_Initialize_Role:
            return Initialize(&inParams->initParams);
        case QTSS_RereadPrefs_Role:
            return RereadPrefs();
        case QTSS_RTSPAuthorize_Role:
            return Authorize(&inParams->rtspAuthParams);
        case QTSS_RTSPSessionClosing_Role:
            return SessionClosing(&inParams->rtspSessionClosingParams);
    }
    return QTSS_NoErr;
}

QTSS_Error Register(QTSS_Register_Params* inParams)
{
    // The spam defense module has one preference, the number of connections
    // to allow per ip addr
    static char*        sIsFirstRequestName = "QTSSSpamDefenseModuleIsFirstRequest";

    // Add text messages attributes
    static char*        sTooManyConnectionsName = "QTSSSpamDefenseModuleTooManyConnections";

    // Do role & attribute setup
    (void)QTSS_AddRole(QTSS_Initialize_Role);
    (void)QTSS_AddRole(QTSS_RereadPrefs_Role);
    
    (void)QTSS_AddRole(QTSS_RTSPAuthorize_Role);
    (void)QTSS_AddRole(QTSS_RTSPSessionClosing_Role);
    
    (void)QTSS_AddStaticAttribute(qtssRTSPSessionObjectType, sIsFirstRequestName, NULL, qtssAttrDataTypeBool16);
    (void)QTSS_IDForAttr(qtssRTSPSessionObjectType, sIsFirstRequestName, &sIsFirstRequestAttr);

    (void)QTSS_AddStaticAttribute(qtssTextMessagesObjectType, sTooManyConnectionsName, NULL, qtssAttrDataTypeCharArray);
    (void)QTSS_IDForAttr(qtssTextMessagesObjectType, sTooManyConnectionsName, &sTooManyConnectionsErr);

    // Tell the server our name!
    static char* sModuleName = "QTSSSpamDefenseModule";
    ::strcpy(inParams->outModuleName, sModuleName);

    return QTSS_NoErr;
}

QTSS_Error Initialize(QTSS_Initialize_Params* inParams)
{
    // Setup module utils
    QTSSModuleUtils::Initialize(inParams->inMessages, inParams->inServer, inParams->inErrorLogStream);
    sPrefs = QTSSModuleUtils::GetModulePrefsObject(inParams->inModule);
    sMutex = NEW OSMutex();
    sHashTable = NEW IPAddrHashTable(277);//277 is prime, I think...
    RereadPrefs();
    return QTSS_NoErr;
}

QTSS_Error RereadPrefs()
{
    QTSSModuleUtils::GetAttribute(sPrefs, "num_conns_per_ip_addr", qtssAttrDataTypeUInt32,
                                &sNumConnsPerIP, &sDefaultNumConnsPerIP, sizeof(sNumConnsPerIP));
    return QTSS_NoErr;
}

QTSS_Error Authorize(QTSS_StandardRTSP_Params* inParams)
{
    static Bool16 sTrue = true;
    
    Bool16* isFirstRequest = NULL;
    UInt32* theIPAddr = NULL;
    UInt32 theLen = 0;
    
    // Only do anything if this is the first request
    (void)QTSS_GetValuePtr(inParams->inRTSPSession, sIsFirstRequestAttr, 0, (void**)&isFirstRequest, &theLen);
    if (isFirstRequest != NULL)
        return QTSS_NoErr;
        
    // Get the IP address of this client.
    (void)QTSS_GetValuePtr(inParams->inRTSPSession, qtssRTSPSesRemoteAddr, 0, (void**)&theIPAddr, &theLen);
    if ((theIPAddr == NULL) || (theLen != sizeof(UInt32)))
    {
        Assert(0);
        return QTSS_NoErr;
    }

    IPAddrTableKey theKey(*theIPAddr);
    
    // This must be atomic
    OSMutexLocker locker(sMutex);

    // Check to see if this client currently has a connection open.
    IPAddrTableElem* theElem = sHashTable->Map(&theKey);
    if (theElem == NULL)
    {
        // Client doesn't have a connetion open currently. Create a map element,
        // and add it into the map.
        theElem = NEW IPAddrTableElem(*theIPAddr);
        sHashTable->Add(theElem);
    }
    
    // Check to see if this client has too many connections open. If it does,
    // return an error, otherwise, allow the connection and increment the
    // refcount.
    if (theElem->GetRefCount() >= sNumConnsPerIP)
        return QTSSModuleUtils::SendErrorResponse(inParams->inRTSPRequest, qtssClientForbidden,
                                                    sTooManyConnectionsErr);
    else
        theElem->IncrementRefCount();
        
    // Mark the request so we'll know subsequent ones aren't the first.
    // Note that we only do this if we've successfully added this client to our map.
    // That way, we only remove it in SessionClosing if we've added it.
    (void)QTSS_SetValue(inParams->inRTSPSession, sIsFirstRequestAttr, 0, &sTrue, sizeof(sTrue));
    
    return QTSS_NoErr;
}

QTSS_Error SessionClosing(QTSS_RTSPSession_Params* inParams)
{
    UInt32* theIPAddr = NULL;
    Bool16* isFirstRequest = NULL;
    UInt32 theLen = 0;
    
    // Only remove this session from the map if it has been added in the first place
    (void)QTSS_GetValuePtr(inParams->inRTSPSession, sIsFirstRequestAttr, 0, (void**)&isFirstRequest, &theLen);
    if (isFirstRequest == NULL)
        return QTSS_NoErr;
        
    // Get the IP address of this client.
    (void)QTSS_GetValuePtr(inParams->inRTSPSession, qtssRTSPSesRemoteAddr, 0, (void**)&theIPAddr, &theLen);
    if ((theIPAddr == NULL) || (theLen != sizeof(UInt32)))
    {
        Assert(0);
        return QTSS_NoErr;
    }

    IPAddrTableKey theKey(*theIPAddr);
    
    // This must be atomic
    OSMutexLocker locker(sMutex);

    // Check to see if this client currently has a connection open.
    IPAddrTableElem* theElem = sHashTable->Map(&theKey);
    if (theElem == NULL)
        return QTSS_NoErr; //this may happen if there is another module denying connections
        
    // Decrement the refcount
    if (theElem->GetRefCount() > 0)
        theElem->DecrementRefCount();
    
    // If the refcount is 0, remove this from the map, and delete it.
    if (theElem->GetRefCount() == 0)
    {
        sHashTable->Remove(theElem);
        delete theElem;
    }
    
    return QTSS_NoErr;      
}
