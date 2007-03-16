#include "TaskHandler.h"
#include <process.h>

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CTaskHandler--------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CTaskHandler::CTaskHandler()
{
	m_hProcessThread = (HANDLE)::_beginthreadex( 0, 0, fnProcessThread, this, 0, NULL );
}

CTaskHandler::~CTaskHandler()
{
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: �������� ����������� �������" );
	m_CloseEv.Set();
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: �������� �������� ������ ���������" );
	WaitForSingleObject( m_hProcessThread, 10000 );
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
		}
	m_csTasks.Leave();
}

unsigned _stdcall CTaskHandler::fnProcessThread( void* param )
{
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: ������ ������ ����������� ������" );
	CTaskHandler* pThis = (CTaskHandler*)param;
	SmartPtr< CTask > pTask;
	try{
		for(;;)
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( pThis->m_CloseEv, 0 ) )
				break;
			
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
				pTask->Execute();
				pTask.Release();
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������� ����������: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������� ����������� ����������" );
	}	
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: ���������� ������ ����������� ������" );
	return 0;
}

