#ifndef __picker_from_file__
#define __picker_from_file__
#include "PlaylistPicker.h"
#include "PLDoubleLinkedList.h"
#include <string.h>

class LoopDetectionListElement {

    public:
        LoopDetectionListElement( const char * name )
        {
            mPathName = new char[ strlen(name) + 1 ];
            
            Assert( mPathName );
            if( mPathName )
                ::strcpy( mPathName, name );
            
        }       
        
        virtual ~LoopDetectionListElement() 
        { 
            if ( mPathName )  
                delete [] mPathName;
        }
        
        char    *mPathName;

};


typedef PLDoubleLinkedList<LoopDetectionListElement> LoopDetectionList;
typedef PLDoubleLinkedListNode<LoopDetectionListElement> LoopDetectionNode;

enum PickerPopulationErrors {

    kPickerPopulateLoopDetected = 1000
    , kPickerPopulateBadFormat
    , kPickerPopulateFileError
    , kPickerPopulateNoMem
    
    , kPickerPopulateNoErr = 0

};

int PopulatePickerFromFile( PlaylistPicker* picker, char* fname, const char* basePath, LoopDetectionList *ldList );
int PopulatePickerFromDir( PlaylistPicker* picker, char* dirPath, int weight = 10 );


#endif
