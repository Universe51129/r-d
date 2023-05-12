
/*
    File:       QTSSRefMovieModule.h

    Contains:   A module that serves an RTSP text ref movie from an HTTP request

*/

#ifndef __QTSSREFMOVIEMODULE_H__
#define __QTSSREFMOVIEMODULE_H__

#include "QTSS.h"

extern "C"
{
    EXPORT QTSS_Error QTSSRefMovieModule_Main(void* inPrivateArgs);
}

#endif //__QTSSREFMOVIEMODULE_H__
