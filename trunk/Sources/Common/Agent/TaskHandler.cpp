#include "precomp.h"
#include "Socket.h"
#include "TaskHandler.h"

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CTaskHandler--------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CTaskHandler::CTaskHandler():m_CloseEv(false)
                            ,m_ProcessThread( SmartPtr<CThreadTask>( new CProcessThreadTask ) )
{
}

CTaskHandler::~CTaskHandler()
{
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: �������� ����������� �������" );
	m_CloseEv.Set();
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: �������� ����������� ������" );
}

void CTaskHandler::AddTask( SmartPtr< CTask > pTask )
{
	Log::instance().Trace( 95, "CTaskHandler::AddTask: ���������� �������: %s", pTask->GetDescription().c_str() );
	CLock lock( m_mtxTasks );
	if( !pTask->Immidiate() )
	{
		Log::instance().Trace( 95, "CTaskHandler::AddTask: ������� ����������� � ������� ����������" );
		m_deqTasks.push_back( pTask );
		m_TaskAddedEv.Set();
	}
}

void CTaskHandler::CProcessThreadTask::Execute( const CEvent& CancelEv )
{
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: ������ ������ ����������� ������" );
	CTaskHandler* pThis = (CTaskHandler*)param;
	SmartPtr< CTask > pTask;
	DWORD dwRes;
	try{
		for(;;)
		{
			HANDLE hEvents[] = { CancelEv, pThis->m_TaskAddedEv };

			if( WAIT_OBJECT_0 == ( dwRes = WaitForMultipleObjects( sizeof( hEvents )/sizeof( hEvents[0] ), hEvents, FALSE, INFINITE ) ) )
				break;
			else if( dwRes != ( WAIT_OBJECT_0 + 1 ) )
				Log::instance().Trace( 10, "CTaskHandler::fnProcessThread: ��������� ������!" );


			pThis->m_mtxTasks.Lock();
				if( pThis->m_deqTasks.size() != 0 )
				{
					pTask = pThis->m_deqTasks.front();
					pThis->m_deqTasks.pop_front();
				}
			pThis->m_mtxTasks.Unlock();

			if( pTask.get() )
			{
				Log::instance().Trace( 10," CTaskHandler::fnProcessThread: ���������� �������: %s", pTask->GetDescription().c_str() );
				pTask->Execute( pThis->m_CloseEv );
				pTask.Release();
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������� ����������: %s", e.what() );
	}/*catch( ... )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������� ����������� ����������" );
	}*/
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: ���������� ������ ����������� ������" );
	return 0;

}

unsigned _stdcall CTaskHandler::fnProcessThread( void* param )
{
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: ������ ������ ����������� ������" );
	CTaskHandler* pThis = (CTaskHandler*)param;
	SmartPtr< CTask > pTask;
	DWORD dwRes;
	try{
		for(;;)
		{
			HANDLE hEvents[] = { pThis->m_CloseEv, pThis->m_TaskAddedEv };

			if( WAIT_OBJECT_0 == ( dwRes = WaitForMultipleObjects( sizeof( hEvents )/sizeof( hEvents[0] ), hEvents, FALSE, INFINITE ) ) )
				break;
			else if( dwRes != ( WAIT_OBJECT_0 + 1 ) )
				Log::instance().Trace( 10, "CTaskHandler::fnProcessThread: ��������� ������!" );


			pThis->m_mtxTasks.Lock();
				if( pThis->m_deqTasks.size() != 0 )
				{
					pTask = pThis->m_deqTasks.front();
					pThis->m_deqTasks.pop_front();
				}
			pThis->m_mtxTasks.Unlock();

			if( pTask.get() )
			{
				Log::instance().Trace( 10," CTaskHandler::fnProcessThread: ���������� �������: %s", pTask->GetDescription().c_str() );
				pTask->Execute( pThis->m_CloseEv );
				pTask.Release();
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������� ����������: %s", e.what() );
	}/*catch( ... )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������� ����������� ����������" );
	}*/
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: ���������� ������ ����������� ������" );
	return 0;
}

