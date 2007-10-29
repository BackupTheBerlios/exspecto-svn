#ifndef __MUTEX
#define __MUTEX

#ifdef WIN32
#else
#include <pthread.h>
#endif


class CMutex
{
public:
    CMutex();
    ~CMutex();

    void Lock();
    bool Lock( int iTime );
    bool TryLock();

    void Unlock();


private:

    CMutex( const CMutex& );
    CMutex& operator=( const CMutex& );

#ifdef WIN32
#else
    pthread_mutex_t m_mtx;
    pthread_cond_t m_cond;
    pthread_mutex_t m_condmtx;
#endif
};

class CLock
{
public:
    CLock( CMutex& mtx ):m_mtx(mtx)
    {
        m_mtx.Lock();
    }

    ~CLock()
    {
        m_mtx.Unlock();
    }
private:
    CLock( const CLock& );
    CLock& operator=( const CLock& );

    CMutex& m_mtx;
};
#endif

