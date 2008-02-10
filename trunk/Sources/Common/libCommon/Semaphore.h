#ifndef SEMAPHORE_H_INCLUDED
#define SEMAPHORE_H_INCLUDED

#ifdef WIN32
#else
#include <pthread.h>
#include <semaphore.h>
#endif

class CSemaphore
{
public:
    CSemaphore( int iResourceCount );
    ~CSemaphore();

    void Release();

    void Wait();
    bool Wait( int iTimeout );
    bool TryWait();

    int GetValue();
private:

    CSemaphore( const CSemaphore& );
    CSemaphore& operator=( const CSemaphore& );

#ifdef WIN32
#else
    sem_t m_sem;
#endif
};

#endif // SEMAPHORE_H_INCLUDED
