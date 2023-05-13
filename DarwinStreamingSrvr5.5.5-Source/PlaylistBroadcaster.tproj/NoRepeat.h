#ifndef __no_repeat__
#define __no_repeat__
#include "PLDoubleLinkedList.h"
#include "SimplePlayListElement.h"

#include "OSHeaders.h"


class NoRepeat : public PLDoubleLinkedList<SimplePlayListElement> {

    public: 
        NoRepeat( UInt32 numNoRepeats );
        virtual ~NoRepeat();
        
        bool IsInList( char* name ); // return true if name is in list, false if not
        bool AddToList( char* name );// return true if could be added to list, no dupes allowd
        PLDoubleLinkedListNode<SimplePlayListElement>* AddToList( PLDoubleLinkedListNode<SimplePlayListElement>* node );

    protected:
        static bool     CompareNameToElement( PLDoubleLinkedListNode<SimplePlayListElement>*node, void *name );
        UInt32          mMaxElements;

};

#endif
