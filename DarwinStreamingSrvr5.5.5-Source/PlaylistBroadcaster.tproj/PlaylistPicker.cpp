#include <stdlib.h>
#include "SafeStdLib.h"
#include <string.h>
#include <stdio.h>
#include "MyAssert.h"
#include "OS.h"
    
#include "PlaylistPicker.h"

PlaylistPicker::PlaylistPicker( UInt32 numBuckets, UInt32 numNoRepeats )
{
    // weighted random ctor
    
    mFirstElement = NULL;
    mNumToPickFrom = 0;
    mBuckets = numBuckets;
    mIsSequentialPicker = false;
    mRecentMoviesListSize = numNoRepeats;

    mRemoveFlag = false;
    mStopFlag = false;
    mLastResult = (UInt32) OS::Milliseconds();

    mPickCounts = new long[numBuckets];
    UInt32 x;
    
    for ( x = 0; x < mBuckets; x++ )
    {   mPickCounts[x] = 0;
        mElementLists[x] = new PLDoubleLinkedList<SimplePlayListElement>;
        Assert( mElementLists[x] );
        
    }
    
    

    mUsedElements = new NoRepeat( numNoRepeats );

    Assert( mUsedElements );
}

PlaylistPicker::PlaylistPicker(bool doLoop)
{
    // sequential ctor
    
    mFirstElement = NULL;
    mIsSequentialLooping = doLoop;
    
    mIsSequentialPicker = true;
    mWhichSequentialBucket = 0;
    mRecentMoviesListSize = 0;
    
    mNumToPickFrom = 0;
    mBuckets = 2;   // alternating used/remaining pick buckets
    mRemoveFlag = false;
    mStopFlag = false;
    fLastPick = NULL;
    
    
    mPickCounts = new long[mBuckets];
    
    
    UInt32  bucketIndex;
    
    for ( bucketIndex = 0; bucketIndex < mBuckets; bucketIndex++ )
    {   
        mPickCounts[bucketIndex] = 0;
        mElementLists[bucketIndex] = new PLDoubleLinkedList<SimplePlayListElement>;
        Assert( mElementLists[bucketIndex] );
        
    }


    mUsedElements = NULL;

}

PlaylistPicker::~PlaylistPicker()
{
    UInt32      bucketIndex;
    
    delete mUsedElements;
    
    for ( bucketIndex = 0; bucketIndex < mBuckets; bucketIndex++ )
    {   
        delete mElementLists[bucketIndex] ;
        
    }
    
    delete [] mPickCounts;
}


UInt32 PlaylistPicker::Random()
{
    UInt32 seed = 1664525L * mLastResult + 1013904223L; //1013904223 is prime .. Knuth D.E. 
    ::srand( seed );
    
    UInt32 result = ::rand();

    mLastResult = result;
    return result;
}


char* PlaylistPicker::PickOne()
{
    
    char*   foundName = NULL;   // pointer to name of pick we find, caller deletes.
    
    
    if ( mIsSequentialPicker )
    {   
        if ( mElementLists[mWhichSequentialBucket]->GetNumNodes() == 0 && mIsSequentialLooping )
        {   // ran out of items switch to other list.
            if ( mWhichSequentialBucket == 0 )
                mWhichSequentialBucket = 1;
            else
                mWhichSequentialBucket = 0;
    
        }
            
        if ( mElementLists[mWhichSequentialBucket]->GetNumNodes() > 0 )
        {
            PLDoubleLinkedListNode<SimplePlayListElement>*  node;
            
            
            node = mElementLists[mWhichSequentialBucket]->GetFirst();
            
            Assert( node );
            
            int nameLen = ::strlen( node->fElement->mElementName );
            
            foundName = new char[ nameLen +1 ];
            
            Assert( foundName );
            
            if ( foundName )
            {   
                int usedBucketIndex;
                
                ::strcpy( foundName, node->fElement->mElementName );
            
                // take him out of the bucket since he's now in play
                mElementLists[mWhichSequentialBucket]->RemoveNode( node );
                
                
                if ( mWhichSequentialBucket == 0 )
                    usedBucketIndex = 1;
                else
                    usedBucketIndex = 0;

                if(!mRemoveFlag)
                    mElementLists[usedBucketIndex]->AddNodeToTail( node );
                else 
                    mNumToPickFrom--;
            
            }
        
        }


    }
    else
    {
        SInt32      bucketIndex;
        UInt32      minimumBucket = 0;
        UInt32      avaiableToPick;
        UInt32      theOneToPick;
        SInt32      topBucket;
        
        
        // find the highest bucket with some elements.
        bucketIndex = this->GetNumBuckets() - 1;
        
        while ( bucketIndex >= 0 &&  mElementLists[bucketIndex]->GetNumNodes() == 0 )
        {
            bucketIndex--;
        }
        
        
        // adjust to 1 based  so we can use MOD
        topBucket = bucketIndex + 1;
    
        if (topBucket > 0)
            minimumBucket = this->Random() % topBucket; // find our minimum bucket        
        
        avaiableToPick = 0;
        
        bucketIndex = minimumBucket;
        
        while ( bucketIndex < topBucket )
        {
            avaiableToPick += mElementLists[bucketIndex]->GetNumNodes();
                
            bucketIndex++;
        }
   
        if ( avaiableToPick )
        {
            theOneToPick = this->Random() % avaiableToPick;     

            bucketIndex = minimumBucket;
            
            while ( bucketIndex < topBucket && foundName == NULL )
            {
                if ( theOneToPick >= mElementLists[bucketIndex]->GetNumNodes() )
                    theOneToPick -= mElementLists[bucketIndex]->GetNumNodes();
                else
                {   
                    foundName = this->PickFromList( mElementLists[bucketIndex], theOneToPick );
                    if ( foundName )
                        mPickCounts[bucketIndex]++;
                }

                bucketIndex++;
            }

            Assert( foundName );
        }
    }
    
    fLastPick = foundName;
    return foundName;

}

void PlaylistPicker::CleanList()
{

    mFirstElement = NULL;
    mNumToPickFrom = 0;
    
    delete mUsedElements;
    mUsedElements = new NoRepeat( mRecentMoviesListSize );
          
    delete [] mPickCounts;
    mPickCounts = new long[mBuckets];
    
    UInt32 x;
    for ( x = 0; x < mBuckets; x++ )
    {   
        mPickCounts[x] = 0;
        delete mElementLists[x];
        mElementLists[x] = new PLDoubleLinkedList<SimplePlayListElement>;
        Assert( mElementLists[x] );
    }
    
};

char*   PlaylistPicker::PickFromList( PLDoubleLinkedList<SimplePlayListElement>* list, UInt32 elementIndex )
{
    PLDoubleLinkedListNode<SimplePlayListElement>*  plNode;
    char*       foundName = NULL;
    
    
    plNode = list->GetNthNode( elementIndex );

    if ( plNode )
    {
        int nameLen = ::strlen(plNode->fElement->mElementName );
        
        foundName = new char[ nameLen +1 ];
        
        Assert( foundName );
        
        if ( foundName )
        {   
            ::strcpy( foundName, plNode->fElement->mElementName );
        
            // take him out of the bucket since he's now in play
            list->RemoveNode( plNode );
            
            mNumToPickFrom--;

            PLDoubleLinkedListNode<SimplePlayListElement>* recycleNode = mUsedElements->AddToList( plNode );

            if ( recycleNode )
                this->AddNode( recycleNode );
        }
    }

    return foundName;

}

bool PlaylistPicker::AddToList( const char* name, int weight )
{
    bool                                            addedSuccesfully;
    PLDoubleLinkedListNode<SimplePlayListElement>*  node;
    SimplePlayListElement*                          element;
    

    node = NULL;
    addedSuccesfully = false;
    element = new SimplePlayListElement(name);
    if (mFirstElement == NULL)
        mFirstElement = element->mElementName;
    
    Assert( element );
    
    
    if ( element )
    {   element->mElementWeight = weight;
        node = new PLDoubleLinkedListNode<SimplePlayListElement>(element);

        Assert( node );
    }
    
    if ( node )
        addedSuccesfully = AddNode(node);
    
        
    return addedSuccesfully;
}

bool PlaylistPicker::AddNode(  PLDoubleLinkedListNode<SimplePlayListElement>* node )
{   
    bool    addSucceeded = false;

    
    Assert( node );
    Assert( node->fElement );
    
    
    if ( mIsSequentialPicker )  // make picks in sequential order, not weighted random
    {
        // add all to bucket 0
        mElementLists[0]->AddNodeToTail( node );
        
        addSucceeded = true;
        mNumToPickFrom++;
    
    }
    else
    {   
        int     weight;
        
        weight = node->fElement->mElementWeight;
    
        // weights are 1 based, correct to zero based for use as array myIndex
        weight--;
        
        Assert( weight >= 0 );
        
        Assert(  (UInt32)weight < mBuckets );
    
        if ( (UInt32)weight < mBuckets )
        {
            // the elements weighting defines the list it is in.
            mElementLists[weight]->AddNode( node );
            
            addSucceeded = true;
            mNumToPickFrom++;
        
        }
    }
    
    return addSucceeded;

}
