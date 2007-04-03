#include "threadspool.h"
#include <process.h>

CThreadsPool::CThreadsPool( int iThreadsCount ):m_Cancel( false )
											   ,m_Exit( false )
{
	structParam* pParams;
	
	for( int i = 0; i < iThreadsCount; i++ )
	{
		pParams = new structParam;
		pParams->pThis = this;
		pParams->iId = i;
		CloseHandle( (HANDLE)_beginthreadex( NULL, 0, ThreadFunc, (void*)pParams, 0, NULL ) );
		m_vecThreadsStates.push_back( SmartPtr<CEvent>( new CEvent(false,1) ) );
		m_vecStateHandles.push_back( *m_vecThreadsStates.back() );
	}
}

CThreadsPool::~CThreadsPool(void)
{
	CancelAllTasks();
	m_Exit.Set();
	WaitForMultipleObjects( (DWORD)m_vecStateHandles.size(), (HANDLE*)&m_vecStateHandles[0], TRUE, 10000 );
	//TODO: Ќужно ли ждать завершени€ всех потоков?
}

void CThreadsPool::AddTask( SmartPtr< CThreadTask > pTask )
{
	CLock lock( m_csTasks );
	m_vecTasks.push_back( pTask );
	m_TaskAdded.Set();
}

void CThreadsPool::CancelAllTasks()
{
	m_Cancel.Set();
}

bool CThreadsPool::WaitAllComplete( int iTimeout )
{
	DWORD dwRes = 0;
	if( ( WAIT_TIMEOUT == ( dwRes = WaitForMultipleObjects( (DWORD)m_vecStateHandles.size(), (HANDLE*)&m_vecStateHandles[0], TRUE, iTimeout ) )
		|| ( WAIT_FAILED == dwRes ) ) )
		return false;
	return true;
}
/*
void CThreadsPool::WaitAllComplete( CEvent& CancelEv )
{
	//m_vecStateHandles.push_back( CancelEv );
	WaitForMultipleObjects( (DWORD)m_vecStateHandles.size(), (HANDLE*)&m_vecStateHandles[0], TRUE, INFINITE );
	//m_vecStateHandles.pop_back();

}
*/
void CThreadsPool::SetCompleted( int iThreadId, bool bCompleted )
{
	CLock lock( m_csThreadsStates );
	if( ( iThreadId >= 0 ) && ( iThreadId < (int)m_vecThreadsStates.size() ) )
		bCompleted?m_vecThreadsStates[ iThreadId ]->Set():m_vecThreadsStates[ iThreadId ]->Reset();
}

SmartPtr< CThreadTask > CThreadsPool::GetTask()
{
	CLock lock( m_csTasks );
	SmartPtr< CThreadTask > pTask = m_vecTasks.back();
	m_vecTasks.pop_back();
	return pTask;
}

unsigned __stdcall CThreadsPool::ThreadFunc( void* param )
{
	structParam* params = (structParam*)param;
	CThreadsPool* pThis = params->pThis;
	int iId = params->iId;
	delete params;
	DWORD dwRes;

	for(;;)
	{
		HANDLE hEvents[] = { pThis->m_TaskAdded, pThis->m_Exit };
		
		if( WAIT_OBJECT_0 + 1 == ( dwRes = WaitForMultipleObjects( sizeof( hEvents )/sizeof( hEvents[0] ), hEvents, FALSE, INFINITE ) )
			|| ( WAIT_FAILED == dwRes ) )
			break;

		pThis->SetCompleted( iId, FALSE );
		
		SmartPtr< CThreadTask > pTask = pThis->GetTask();

		pTask->Execute( pThis->m_Cancel );

		pThis->SetCompleted( iId, TRUE );
	}
	return 0;
}
