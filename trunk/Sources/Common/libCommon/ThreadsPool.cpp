#include "ThreadsPool.h"
#include <sstream>
#include "Os_Spec.h"


CThreadsPool::CThreadsPool( int iThreadsCount ):m_Cancel(false)
											   ,m_Exit(false)
											   ,m_sem(0)
{
	for( int i = 0; i < iThreadsCount; i++ )
	{
		m_vecThreads.push_back( SmartPtr<WorkingThread>( new WorkingThread( this, i ) ) );
		m_vecThreadsStates.push_back( SmartPtr<CEvent>( new CEvent(false,true) ) );
		m_vecThreads[m_vecThreads.size()-1]->start();
	}
}

CThreadsPool::~CThreadsPool(void)
{
	CancelAllTasks();
	m_Exit.Set();

	//TODO:Здесь нужно ждать корректного завершения всех потоков,
	//в этом случае все задачи успеют корректно завершиться и не
	//будут обращаться к удаленным ресурсам
	for( std::vector< SmartPtr<WorkingThread> >::iterator It = m_vecThreads.begin(); It != m_vecThreads.end(); It++ )
	{
		(*It)->signal();
		for( int i = 0; i < 10; i++ )
		{
			if( (*It)->get_finished() )
				break;
			pt::psleep(100);
		}
	}
}

void CThreadsPool::AddTask( SmartPtr< CThreadTask > pTask )
{
	pt::scopelock lock( m_mtTasks );
	m_vecTasks.push_back( pTask );
	Log::instance().Trace( 0, "%s: %d", __FUNCTION__, m_vecTasks.size() );
	m_sem.post();
}

SmartPtr< CThreadTask > CThreadsPool::GetTask()
{
	Log::instance().Trace( 0, "%s: 1", __FUNCTION__ );
	pt::scopelock lock( m_mtTasks );
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
		if( !(*It)->Wait(0) )
		{
			ssTmp << i << " = Busy; ";
		}
	}
	Log::instance().Trace( 0, "CThreadsPool::LogStates: %s", ssTmp.str().c_str() );
}

bool CThreadsPool::WaitAllComplete( CEvent& CancelEv )
{
	LogStates();
	m_mtTasks.lock();
	if( !m_vecTasks.empty() )
	{
        m_TasksEmpty.Reset();
        m_mtTasks.unlock();
		Log::instance().Trace( 0, "CThreadsPool::WaitAllComplete: Wait for empty" );

		for(;;)
		{
			if( m_TasksEmpty.Wait(0) )
				break;
			if( CancelEv.Wait(100) )
			{
				Log::instance().Trace( 0, "CThreadsPool::WaitAllComplete: canceled" );
				return false;
			}
		}
	}else
		m_mtTasks.unlock();
	Log::instance().Trace( 0, "CThreadsPool::WaitAllComplete: Wait for complete" );
	for( std::vector< SmartPtr<CEvent> >::iterator It = m_vecThreadsStates.begin(); It != m_vecThreadsStates.end(); It++ )
	{
		for(;;)
		{
			if( (*It)->Wait(0) )
				break;
			if( CancelEv.Wait(100) )
			{
				Log::instance().Trace( 0, "CThreadsPool::WaitAllComplete: canceled2" );
				return false;
			}
		}
	}
	LogStates();
	return true;
}

void CThreadsPool::SetCompleted( int iThreadId, bool bCompleted )
{
	pt::scopelock lock( m_mtThreadsStates );
	if( ( iThreadId >= 0 ) && ( iThreadId < (int)m_vecThreadsStates.size() ) )
		bCompleted?m_vecThreadsStates[ iThreadId ]->Set():m_vecThreadsStates[ iThreadId ]->Reset();
}

void CThreadsPool::WorkingThread::execute()
{
	bool bSignaled = false;
	for(;;)
	{
		for(;;)
		{
			if( get_signaled() )
			{
				bSignaled = true;
				break;
			}

			if( m_pThis->m_sem.trywait() )
				break;
            pt::psleep(100);
		}
		if( bSignaled )
			break;


		m_pThis->SetCompleted( m_iId, false );

		SmartPtr< CThreadTask > pTask = m_pThis->GetTask();

		if( pTask.get() )
			pTask->Execute( m_pThis->m_Cancel );

		m_pThis->SetCompleted( m_iId, true );
	}
}

