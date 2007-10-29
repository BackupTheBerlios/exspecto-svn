#include "Semaphore.h"
#include <errno.h>
#include <sys/time.h>

CSemaphore::CSemaphore( int iResourceCount )
{
    sem_init( &m_sem, 0, iResourceCount );
}

CSemaphore::~CSemaphore()
{
    sem_destroy( &m_sem );
}

void CSemaphore::Release()
{
    sem_post( &m_sem );
}

void CSemaphore::Wait()
{
    sem_wait( &m_sem );
}

bool CSemaphore::TryWait()
{
    return (0 == sem_trywait( &m_sem ))?true:false;
}

bool CSemaphore::Wait( int iTimeout )
{
    timeval now;
    timespec timeout;
    gettimeofday( &now, NULL );
    timeout.tv_sec = now.tv_sec + iTimeout/1000;
    timeout.tv_nsec = now.tv_usec * 1000 + (iTimeout%1000)*1000;
    return ( 0== sem_timedwait( &m_sem, &timeout ) )?true:false;
}

int CSemaphore::GetValue()
{
    int iRes = 0;
    sem_getvalue( &m_sem, &iRes );
    return iRes;
}
