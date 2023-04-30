#ifndef __DSS_STOPWATCH__
#define __DSS_STOPWATCH__
//#include "DssStopwatch.h"

#include "OS.h"


class DssEggtimer {
    public:
        enum { kDurationNeverExpire = -1 };
        
        DssEggtimer( SInt64 inMilliseconds ) { fTimerDuration = inMilliseconds; Reset(); }
        
        void OneShotSetTo( SInt64 inMilliseconds )
        {
            fExpirationMilliseconds =  OS::Milliseconds() + inMilliseconds;
        }
        void Reset() 
        { 
            fExpirationMilliseconds =  OS::Milliseconds() + fTimerDuration; 
        }
        
        void ResetTo(SInt64 inMilliseconds) 
        { 
            fTimerDuration = inMilliseconds;
            this->Reset();
        }
        
        Bool16 Expired() 
        { 
            return fExpirationMilliseconds <= OS::Milliseconds(); 
        }
        SInt64  MaxDuration() { return fTimerDuration; }
        
    private:
        SInt64  fTimerDuration; 
        SInt64  fExpirationMilliseconds;

};

class DssMillisecondStopwatch {

    public:
        DssMillisecondStopwatch() :
        fIsStarted(false)
        , fTimerDuration(-1)
        {}
        ;
        void Start() { fStartedAt = OS::Milliseconds(); fIsStarted = true; }
        void Stop()  { fTimerDuration = OS::Milliseconds() - fStartedAt; }
        
        SInt64  Duration() { return fTimerDuration; }

    private:
        Bool16  fIsStarted;
        SInt64  fTimerDuration; 
        SInt64  fStartedAt; 
};

class DssDurationTimer {

    public:
        DssDurationTimer() { fStartedAtMsec = OS::Milliseconds(); }
        void Reset() { fStartedAtMsec = OS::Milliseconds(); }
        void ResetToDuration( SInt64 inDurationInMsec ) { fStartedAtMsec = OS::Milliseconds() - inDurationInMsec; }
        SInt64 DurationInMilliseconds() { return OS::Milliseconds() - fStartedAtMsec; }
        SInt64 DurationInSeconds() { return (OS::Milliseconds() - fStartedAtMsec) / (SInt64)1000; }
        

    private:
        SInt64  fStartedAtMsec; 
};


#endif

