#include "threadspool.h"
#include <process.h>
#include <sstream>


CThreadsPool::CThreadsPool( int iThreadsCount ):m_Cancel( false )
											   ,m_Exit( false )
{
	structParam* pParams;
	
	for( int i = 0; i < iThreadsCount; i++ )
	{
		pParams = new structParam;
		pParams->pThis = this;
		pParams->iId = i;
		m_vecThreads.push_back( (HANDLE)_beginthreadex( NULL, 0, ThreadFunc, (void*)pParams, 0, NULL ) );
		m_vecThreadsStates.push_back( SmartPtr<CEvent>( new CEvent(false,1) ) );
	}
}

CThreadsPool::~CThreadsPool(void)
{
	CancelAllTasks();
	m_Exit.Set();
	
	//TODO:«десь нужно ждать корректного завершени€ всех потоков,
	//в этом случае все задачи успеют корректно завершитьс€ и не
	//будут обращатьс€ к удаленным ресурсам
	//Ётот wait не работает,нужно провести исследование
	WaitForMultipleObjects( (DWORD)m_vecThreads.size(), (HANDLE*)&m_vecThreads[0], TRUE, 10000 );
	Sleep(10000);
}

void CThreadsPool::AddTask( SmartPtr< CThreadTask > pTask )
{
	CLock lock( m_csTasks );
	m_vecTasks.push_back( pTask );
	m_TasksSem.Release();
}

SmartPtr< CThreadTask > CThreadsPool::GetTask()
{
	CLock lock( m_csTasks );
	Log::instance().Trace( 0, "CThreadsPool::GetTask: %d", m_vecTasks.size() );
	SmartPtr< CThreadTask > pTask;
	if( !m_vecTasks.empty() )
	{
		pTask = m_vecTasks.back();
		m_vecTasks.pop_back();
		if( m_vecTasks.empty() )
		{
			Log::instance().Trace( 0, "CThreadsPool::GetTask: TaskEmpty" );
			m_TasksEmpty.Set();
		}
	}else
	{
        pTask = SmartPtr<CThreadTask>( NULL );
	}
	return pTask;
}

void CThreadsPool::CancelAllTasks()
{
	m_Cancel.Set();
}
void CThreadsPool::LogStates()
{
	int i = 0;
	std::stringstream ssTmp;
	for( std::vector< SmartPtr< CEvent > >::iterator It = m_vecThreadsStates.begin(); It != m_vecThreadsStates.end(); It++, i++ )
	{
		if( WAIT_TIMEOUT == WaitForSingleObject( *(*It), 0 ) )
		{
			ssTmp << i << " = Busy; ";
		}
	}
	Log::instance().Trace( 0, "CThreadsPool::LogStates: %s", ssTmp.str().c_str() );
}
/*
bool CThreadsPool::WaitAllComplete( int iTimeout )
{
	//Ќепон€тно почему не работает
	//DWORD dwRes = 0;
	//LogStates();
	//if( ( WAIT_TIMEOUT == ( dwRes = WaitForMultipleObjects( (DWORD)m_vecStateHandles.size(), (HANDLE*)&m_vecStateHandles[0], TRUE, iTimeout ) )
	//	|| ( WAIT_FAILED == dwRes ) ) )
	//	return false;
	//LogStates();
	//return true;
	*/
/*	LogStates();
	if( !m_vecTasks.empty() )
	{
        m_TasksEmpty.Reset();
		if( WAIT_OBJECT_0 != WaitForSingleObject( m_TasksEmpty, iTimeout ) )
			return false;
	}
	for( std::vector< SmartPtr<CEvent> >::iterator It = m_vecThreadsStates.begin(); It != m_vecThreadsStates.end(); It++ )
		if( WAIT_OBJECT_0 != WaitForSingleObject( *(*It), iTimeout ) )
			return false;
	LogStates();
	return true;

}
*/
bool CThreadsPool::WaitAllComplete( CEvent& CancelEv )
{
	HANDLE hEvents[] = { CancelEv, m_TasksEmpty };
	LogStates();
	if( !m_vecTasks.empty() )
	{
        m_TasksEmpty.Reset();
		Log::instance().Trace( 0, "CThreadsPool::WaitAllComplete: Wait for empty" );
		if( (WAIT_OBJECT_0+1) != WaitForMultipleObjects( sizeof( hEvents )/sizeof( hEvents[0] ), hEvents, FALSE, INFINITE ) )
		{
			Log::instance().Trace( 0, "CThreadsPool::WaitAllComplete: canceled" );
			return false;
		}
	}
	Log::instance().Trace( 0, "CThreadsPool::WaitAllComplete: Wait for complete" );
	for( std::vector< SmartPtr<CEvent> >::iterator It = m_vecThreadsStates.begin(); It != m_vecThreadsStates.end(); It++ )
	{
		hEvents[1] = *(*It);
		if( (WAIT_OBJECT_0+1) != WaitForMultipleObjects( 2, hEvents, FALSE, INFINITE ) )
		{
			Log::instance().Trace( 0, "CThreadsPool::WaitAllComplete: canceled2" );
			return false;
		}
	}
	LogStates();
	return true;
}

void CThreadsPool::SetCompleted( int iThreadId, bool bCompleted )
{
	CLock lock( m_csThreadsStates );
	if( ( iThreadId >= 0 ) && ( iThreadId < (int)m_vecThreadsStates.size() ) )
		bCompleted?m_vecThreadsStates[ iThreadId ]->Set():m_vecThreadsStates[ iThreadId ]->Reset();
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
		HANDLE hEvents[] = { pThis->m_TasksSem, pThis->m_Exit };

		if( WAIT_OBJECT_0 + 1 == ( dwRes = WaitForMultipleObjects( sizeof( hEvents )/sizeof( hEvents[0] ), hEvents, FALSE, INFINITE ) )
			|| ( WAIT_FAILED == dwRes ) )
			break;

		pThis->SetCompleted( iId, FALSE );
		
		SmartPtr< CThreadTask > pTask = pThis->GetTask();

		if( pTask.get() )
			pTask->Execute( pThis->m_Cancel );

		pThis->SetCompleted( iId, TRUE );
	}

	return 0;
}
