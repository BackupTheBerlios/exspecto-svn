#include "Thread.h"

CThread::~CThread()
{
    Stop();
}

void CThread::Start()
{
    pthread_attr_t attr;
    pthread_attr_init( &attr );
    pthread_attr_setdetachstate ( &attr, PTHREAD_CREATE_DETACHED );
    pthread_create ( &m_thread, &attr, &CThread::ThreadFunc, this );
    pthread_attr_destroy ( &attr );

}

void CThread::Stop()
{
    m_StopEv.Set();
}

void* CThread::ThreadFunc( void* param )
{
    CThread* pThis = (CThread*)param;
    if( NULL != pThis->m_pTask.get() )
        pThis->m_pTask->Execute( pThis->m_StopEv );
    return NULL;
}
