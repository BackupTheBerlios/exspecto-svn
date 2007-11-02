#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include "Event.h"
#include "SmartPtr.hpp"

class CThreadTask
{
public:
	virtual ~CThreadTask(){};

	virtual void Execute( CEvent& CancelEvent ) = 0;

};

class CThread
{
public:
    CThread( SmartPtr<CThreadTask> pTask, bool bAutoStart = true ):m_pTask( pTask )
                                                                  ,m_bWorking(true)
    {
        if( bAutoStart )
            Start();
    }

    ~CThread();

    void Start();

    void Stop();

    bool IsWorking(){ return m_bWorking; }

private:
    CThread( const CThread& );
    CThread& operator=( const CThread& );

    SmartPtr<CThreadTask> m_pTask;

    CEvent m_StopEv;

    bool m_bWorking;

#ifdef WIN32
#else
    static void* ThreadFunc( void* param );
    pthread_t m_thread;
#endif
};

#endif // THREAD_H_INCLUDED
