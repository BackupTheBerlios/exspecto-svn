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
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: Закрытие обработчика заданий" );
	m_CloseEv.Set();
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: Ожидание закрытия потока обработки" );
	WaitForSingleObject( m_hProcessThread, 10000 );
	CloseHandle( m_hProcessThread );
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: Закрытие обработчика команд" );
}
	
void CTaskHandler::AddTask( SmartPtr< CTask > pTask )
{
	Log::instance().Trace( 95, "CTaskHandler::AddTask: Добавление задания: %s", pTask->GetDescription().c_str() );
	m_csTasks.Enter();
		if( !pTask->Immidiate() )
		{
			Log::instance().Trace( 95, "CTaskHandler::AddTask: Задание поставленно в очередь выполнения" );
			m_deqTasks.push_back( pTask );
		}
	m_csTasks.Leave();
}

unsigned _stdcall CTaskHandler::fnProcessThread( void* param )
{
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: Запуск потока обработчика команд" );
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
				Log::instance().Trace( 10," CTaskHandler::fnProcessThread: Выполнение задания: %s", pTask->GetDescription().c_str() );
				pTask->Execute();
				pTask.Release();
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: Возникло исключение: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: Возникло неизвестное исключение" );
	}	
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: Завершение потока обработчика команд" );
	return 0;
}

