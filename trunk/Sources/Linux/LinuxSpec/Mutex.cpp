#include "Mutex.h"
#include "errno.h"
#include <sys/time.h>

CMutex::CMutex()
{
    pthread_mutex_init( &m_mtx, NULL );
    pthread_mutex_init( &m_condmtx, NULL );
    pthread_cond_init( &m_cond, NULL );
}

CMutex::~CMutex()
{
    pthread_mutex_destroy( &m_mtx );
    pthread_mutex_destroy( &m_condmtx );
    pthread_cond_destroy( &m_cond );
}

void CMutex::Lock()
{
    pthread_mutex_lock( &m_mtx );
}

void CMutex::Unlock()
{
    pthread_mutex_lock( &m_condmtx );
        pthread_mutex_unlock( &m_mtx );
        pthread_cond_signal( &m_cond );
    pthread_mutex_unlock( &m_condmtx );
}

bool CMutex::TryLock()
{
    bool bRes;
    pthread_mutex_lock( &m_condmtx );
        bRes = ( EBUSY == pthread_mutex_trylock( &m_mtx ) )?false:true;
    pthread_mutex_unlock( &m_condmtx );
    return bRes;
}

bool CMutex::Lock( int iTimeOut )
{
    timeval now;
    timespec timeout;
    bool bRes = false;

    pthread_mutex_lock( &m_condmtx );
        gettimeofday( &now, NULL );
        timeout.tv_sec = now.tv_sec + iTimeOut/1000;
        timeout.tv_nsec = now.tv_usec * 1000 + (iTimeOut%1000)*1000;
        bRes = ( EBUSY == pthread_mutex_trylock( &m_mtx ) )?false:true;
        if( !bRes
            && ( ( bRes = ( ETIMEDOUT == pthread_cond_timedwait( &m_cond, &m_condmtx, &timeout ) )?false:true ) ) )
                pthread_mutex_lock( &m_mtx );
    pthread_mutex_unlock( &m_condmtx );
    return bRes;
}

