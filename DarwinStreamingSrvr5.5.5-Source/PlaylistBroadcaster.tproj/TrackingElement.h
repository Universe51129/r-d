#ifndef __tracking_element__
#define __tracking_element__

#include <unistd.h>
#include <string.h>
#include "OSHeaders.h"
#include "MyAssert.h"

class TrackingElement {

    public:
        TrackingElement( pid_t pid, const char * name )
        {
            mName = new char[ strlen(name) + 1 ];
            
            Assert( mName );
            if( mName )
                strcpy( mName, name );
            
            mPID = pid;
            
        }       
        
        virtual ~TrackingElement() 
        { 
            if ( mName )  
                delete [] mName;
        }
        
        char    *mName;
        pid_t   mPID;

};

#endif
