#include "precomp.h"
#include "Socket.h"
#include <process.h>
#include "TaskHandler.h"

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CTaskHandler--------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CTaskHandler::CTaskHandler():m_CloseEv(false)
{
	m_hProcessThread = (HANDLE)::_beginthreadex( NULL, 0, fnProcessThread, this, 0, NULL );
}

CTaskHandler::~CTaskHandler()
{
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: �������� ����������� �������" );
	m_CloseEv.Set();
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: �������� �������� ������ ���������" );
	WaitForSingleObject( m_hProcessThread, 20000 );
	CloseHandle( m_hProcessThread );
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: �������� ����������� ������" );
}
	
void CTaskHandler::AddTask( SmartPtr< CTask > pTask )
{
	Log::instance().Trace( 95, "CTaskHandler::AddTask: ���������� �������: %s", pTask->GetDescription().c_str() );
	m_csTasks.Enter();
		if( !pTask->Immidiate() )
		{
			Log::instance().Trace( 95, "CTaskHandler::AddTask: ������� ����������� � ������� ����������" );
			m_deqTasks.push_back( pTask );
			m_TaskAddedEv.Set();
		}
	m_csTasks.Leave();
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

			
			pThis->m_csTasks.Enter();
				if( pThis->m_deqTasks.size() != 0 )
				{
					pTask = pThis->m_deqTasks.front();
					pThis->m_deqTasks.pop_front();
				}
			pThis->m_csTasks.Leave();
			
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
