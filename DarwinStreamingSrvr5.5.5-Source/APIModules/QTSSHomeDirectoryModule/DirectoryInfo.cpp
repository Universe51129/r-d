/*
This code implements a session list, 
which mainly includes functions to add and remove sessions and calculate the current total bandwidth.
The SessionListElement class represents the session element and contains a pointer to QTSS_ClientSessionObject 
and some functions to determine if the sessions are the same, 
get the bit rate of the current session, etc.

The DirectoryInfo class is the session list class and contains a list of sessions
and a mutex lock to protect them when adding or removing sessions.
 
With the implementation of these functions,
the session list can be managed efficiently and the current total bandwidth can be calculated.
*/
/*
    File:       DirectoryInfo.cpp

    Contains:   Implementation of class defined in .h file
                    
    
*/

#include "DirectoryInfo.h"

Bool16 SessionListElement::Equal(QTSS_ClientSessionObject* inSessionPtr)
{
	UInt32 *theSessionID = 0;
	UInt32 *inSessionID = 0;
	UInt32 theLen = 0;
	
	(void)QTSS_GetValuePtr(fSession, qtssCliSesCounterID, 0, (void **)&theSessionID, &theLen);
	Assert(theLen != 0);
	(void)QTSS_GetValuePtr(*inSessionPtr, qtssCliSesCounterID, 0, (void **)&inSessionID, &theLen);
	Assert(theLen != 0);
	
	if (*theSessionID == *inSessionID)
		return true;
	
	return false;
}

UInt32 SessionListElement::CurrentBitRate()
{
	UInt32 *theBitRate = 0;
	UInt32 theLen = 0;
	
	(void)QTSS_GetValuePtr(fSession, qtssCliSesCurrentBitRate, 0, (void **)&theBitRate, &theLen);
	Assert(theLen != 0);
		
	return *theBitRate;
}

// find a specific session in the session list
static bool IsSession(PLDoubleLinkedListNode<SessionListElement>* node,  void* userData)
{
	/*
        used by ForEachUntil to find a SessionListElement with a given Session
        userData is a pointer to the QTSS_ClientSessionObject we want to find
        
    */
	return node->fElement->Equal((QTSS_ClientSessionObject *)userData);
}

//to calculate the total bandwidth. 
static void AddCurrentBitRate(PLDoubleLinkedListNode<SessionListElement>* node,  void* userData)
{
	*(UInt64 *)userData += node->fElement->CurrentBitRate();
}

DirectoryInfo::~DirectoryInfo()
{
	fMutex.Lock();
	fClientSessionList->ClearList();
	fNumSessions = 0;
	fHomeDir.Delete();
	fMutex.Unlock();
}

void DirectoryInfo::AddSession(QTSS_ClientSessionObject *sessionPtr)
{
	fMutex.Lock();
	
	SessionListElement *theElement = NEW SessionListElement(sessionPtr);
	PLDoubleLinkedListNode<SessionListElement> *sessionNode = new PLDoubleLinkedListNode<SessionListElement> (theElement);
	fClientSessionList->AddNode(sessionNode);
	fNumSessions++;
	
	fMutex.Unlock();	
}

void DirectoryInfo::RemoveSession(QTSS_ClientSessionObject *sessionPtr)
{
	fMutex.Lock();
	
	PLDoubleLinkedListNode<SessionListElement> *node = NULL;
    
    node = fClientSessionList->ForEachUntil(IsSession, (void *)sessionPtr);
	if (node != NULL)
	{
		fClientSessionList->RemoveNode(node);
		fNumSessions--;
	}
	
	fMutex.Unlock();	
}

//calculate the current total bandwidth, 
UInt64 DirectoryInfo::CurrentTotalBandwidthInKbps()
{
	fMutex.Lock();
	UInt64 totalBandwidth = 0;
	fClientSessionList->ForEach(AddCurrentBitRate, &totalBandwidth);
	fMutex.Unlock();
		
	return (totalBandwidth/1024);
}
