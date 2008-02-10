#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED
#ifdef WIN32
#else
#include "Mutex.h"
#endif

class CEvent
{
public:
    CEvent( bool bAutoreset = true, bool bInitialState = false );
    ~CEvent();

    void Set();

    void Reset();

    void Wait();

    bool Wait( int iTimeout );

    bool TryWait();

private:

    CEvent( const CEvent& );
    CEvent& operator=( const CEvent& );

#ifdef WIN32
#else
    CMutex m_mtx;
    CMutex m_lockmtx;
    bool m_bAutoReset;
    bool m_bInitialState;
#endif
};

#endif // EVENT_H_INCLUDED
