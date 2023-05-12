/*
    File:       DirectoryInfo.h

    Contains:   Stores an array of client sessions, # of client sessions,
				and the home directory
                    
    
*/
#ifndef _DIRECTORYINFO_H_
#define _DIRECTORYINFO_H_
#include "QTSS.h"
#include "StrPtrLen.h"
#include "OSRef.h"
#include "OSMemory.h"
#include "PLDoubleLinkedList.h"

//represents a client session object and contains a member variable fSession, 
//which represents a client session object; 
//the Equal function is used to compare two client session objects to see if they are equal,
//and the CurrentBitRate function returns the current bit rate.
class SessionListElement {
    public:
        SessionListElement(QTSS_ClientSessionObject *inSessionPtr) { fSession = *inSessionPtr; }
		
        virtual ~SessionListElement() { fSession = NULL; }
		
		Bool16 Equal(QTSS_ClientSessionObject* inSessionPtr);
        UInt32 CurrentBitRate();
	
	private:
		QTSS_ClientSessionObject fSession;
};

//The DirectoryInfo class represents a DirectoryInfo object 
//and contains a member variable fRef that represents a reference to the DirectoryInfo object; 
//fMutex is a mutex lock; 
//fClientSessionList is a two-way chain table that stores the client session objects; 
//The GetRef function returns a reference to the directory information object;
//the AddSession and RemoveSession functions are used to add and remove client session objects;
//and the CurrentTotalBandwidthInKbps function returns the current total bandwidth.
class DirectoryInfo
{
    public:
	DirectoryInfo(StrPtrLen *inHomeDir):fNumSessions(0), fHomeDir(inHomeDir->GetAsCString())
	{
		fClientSessionList = NEW PLDoubleLinkedList<SessionListElement>;
		fRef.Set(fHomeDir, (void *)this);
	}
	
	~DirectoryInfo();
	OSRef*	GetRef() { return &fRef; }
	void	AddSession(QTSS_ClientSessionObject *sessionPtr);
	void	RemoveSession(QTSS_ClientSessionObject *sessionPtr);
	UInt64	CurrentTotalBandwidthInKbps();
	UInt32	NumSessions() { return fNumSessions; }

	private:
		OSRef											fRef;
		OSMutex											fMutex;
		PLDoubleLinkedList<SessionListElement>			*fClientSessionList;
		UInt32											fNumSessions;
		StrPtrLen										fHomeDir;
};


#endif // _DIRECTORYINFO_H_ 
