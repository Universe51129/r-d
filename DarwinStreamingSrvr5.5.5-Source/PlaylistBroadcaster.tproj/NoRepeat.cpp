#include <stdlib.h>
#include "SafeStdLib.h"
#include <string.h>


#include "NoRepeat.h"

NoRepeat::NoRepeat ( UInt32 numNoRepeats )
    : PLDoubleLinkedList<SimplePlayListElement>()
{
    mMaxElements = numNoRepeats;
}

NoRepeat::~NoRepeat()
{
    // we have nothing to do, just let the PLDoubleLinkedList clear itself
}

bool NoRepeat::CompareNameToElement( PLDoubleLinkedListNode<SimplePlayListElement>* node, void* name )
{
    if ( !::strcmp( node->fElement->mElementName, (const char*)name ) )
        return true;
    
    return false;
}


bool NoRepeat::IsInList( char* name )
{
    PLDoubleLinkedListNode<SimplePlayListElement>* whichElement;
    
    whichElement = ForEachUntil( CompareNameToElement, (void*)name );
    
    if ( whichElement )
        return true;
    
    return false;
    
}


PLDoubleLinkedListNode<SimplePlayListElement>* NoRepeat::AddToList( PLDoubleLinkedListNode<SimplePlayListElement>* node )
{
    AddNode(node);
    PLDoubleLinkedListNode<SimplePlayListElement>* oldTail = NULL;
    
    if ( fNumNodes > mMaxElements )
    {   oldTail = fTail;
        this->RemoveNode( fTail );
    }
    
    return oldTail;
}

bool NoRepeat::AddToList( char* name )
{
    Assert( false );
    
    bool addedSuccesfully = false;

    if ( !this->IsInList( name ) )
    {
        SimplePlayListElement*                          element;
        PLDoubleLinkedListNode<SimplePlayListElement>*  node = NULL;

        element = new SimplePlayListElement(name);
        
        Assert( element );
        
        if ( element )
            node = new PLDoubleLinkedListNode<SimplePlayListElement>(element);
    
        Assert( node );
        
        if ( node )
        {   
            PLDoubleLinkedListNode<SimplePlayListElement>* deadNode;
            
            deadNode = this->AddToList(node);
            
            delete deadNode;
            addedSuccesfully = true;
        
        }
        
    }
    
    return addedSuccesfully;
}

