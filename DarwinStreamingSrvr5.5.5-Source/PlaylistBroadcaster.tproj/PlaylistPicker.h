#ifndef __playlist_picker__
#define __playlist_picker__
#include "PLDoubleLinkedList.h"
#include "SimplePlayListElement.h"

#include "OSHeaders.h"

#include "NoRepeat.h"

class PlaylistPicker
{

    public:
        enum { kMaxBuckets = 10 };
        
        PlaylistPicker(bool doLoop);
        PlaylistPicker( UInt32 numBuckets, UInt32 numNoRepeats );
        virtual ~PlaylistPicker();
        void        CleanList(); 
        bool        AddToList( const char *name, int weight );
        bool        AddNode( PLDoubleLinkedListNode<SimplePlayListElement> *node );
        char*       PickOne();
        char*       LastPick() { return fLastPick; }
        UInt32      GetNumBuckets() { return mBuckets; }
        UInt32      GetNumMovies() { return mNumToPickFrom; }

        bool        mRemoveFlag;
        bool        mStopFlag;
        long*       mPickCounts;
        long        mNumToPickFrom;
        UInt32      mRecentMoviesListSize;
        char*       fLastPick;
        PLDoubleLinkedList<SimplePlayListElement>*  GetBucket( UInt32 myIndex ) { return mElementLists[myIndex]; }
        
        char*       GetFirstFile() { return mFirstElement; }

    protected:
        
        bool        mIsSequentialPicker;    // picker picks sequentially?
        bool        mIsSequentialLooping;   // loop over and over?
        int         mWhichSequentialBucket; // sequential picker picks  from list0 or list1?
        
        UInt32      Random();
        UInt32      mLastResult;
        
        char*       PickFromList( PLDoubleLinkedList<SimplePlayListElement>* list, UInt32 elementIndex );
        
        PLDoubleLinkedList<SimplePlayListElement>* mElementLists[kMaxBuckets];
        
        UInt32                                  mBuckets;
        NoRepeat                                *mUsedElements;
        char*       mFirstElement;
};



#endif
