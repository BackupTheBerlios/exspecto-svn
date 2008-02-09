#include "precomp.h"
#include "Socket.h"
#include "TaskHandler.h"

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CTaskHandler--------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CTaskHandler::CTaskHandler():m_ProcessThread( SmartPtr<CThreadTask>( new CProcessThreadTask(this) ) )
{
}

CTaskHandler::~CTaskHandler()
{
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: œô¨·œô¨Ðœô¨Úœô¨àœô¨ëœô¨âœô¨Øœô¨Õ œô¨Þœô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨çœô¨Øœô¨Úœô¨Ð œô¨Úœô¨Þœô¨Üœô¨Ðœô¨Ýœô¨Ô" );
}

void CTaskHandler::AddTask( SmartPtr< CTask > pTask )
{
	Log::instance().Trace( 95, "CTaskHandler::AddTask: œô¨´œô¨Þœô¨Ñœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï: %s", pTask->GetDescription().c_str() );
	CLock lock( m_mtxTasks );
	if( !pTask->Immidiate() )
	{
		Log::instance().Trace( 95, "CTaskHandler::AddTask: œô¨·œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨ßœô¨Þœô¨áœô¨âœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨Ýœô¨Þ œô¨Ò œô¨Þœô¨çœô¨Õœô¨àœô¨Õœô¨Ôœô¨ì œô¨Òœô¨ëœô¨ßœô¨Þœô¨Ûœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨ï" );
		m_deqTasks.push_back( pTask );
		m_TaskAddedEv.Set();
	}
}

void CTaskHandler::CProcessThreadTask::Execute( CEvent& CancelEv )
{
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: œô¨·œô¨Ðœô¨ßœô¨ãœô¨áœô¨Ú œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Ð œô¨Þœô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨çœô¨Øœô¨Úœô¨Ð œô¨Úœô¨Þœô¨Üœô¨Ðœô¨Ýœô¨Ô" );
	SmartPtr< CTask > pTask;
	try{
		for(;;)
		{
            bool bCanceled = false;
            for(;;)
            {
                if( m_pThis->m_TaskAddedEv.Wait(0) )
                    break;
                if( CancelEv.Wait(100) )
                {
                    bCanceled = true;
                    break;
                }
            }

            if( bCanceled )
                break;

			m_pThis->m_mtxTasks.Lock();
				if( m_pThis->m_deqTasks.size() != 0 )
				{
					pTask = m_pThis->m_deqTasks.front();
					m_pThis->m_deqTasks.pop_front();
				}
			m_pThis->m_mtxTasks.Unlock();

			if( pTask.get() )
			{
				Log::instance().Trace( 10," CTaskHandler::fnProcessThread: œô¨²œô¨ëœô¨ßœô¨Þœô¨Ûœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï: %s", pTask->GetDescription().c_str() );
				pTask->Execute( CancelEv );
				pTask.Release();
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ: %s", e.what() );
	}/*catch( ... )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ" );
	}*/
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: œô¨·œô¨Ðœô¨Òœô¨Õœô¨àœô¨èœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Ð œô¨Þœô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨çœô¨Øœô¨Úœô¨Ð œô¨Úœô¨Þœô¨Üœô¨Ðœô¨Ýœô¨Ô" );
}
/*
unsigned _stdcall CTaskHandler::fnProcessThread( void* param )
{
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: œô¨·œô¨Ðœô¨ßœô¨ãœô¨áœô¨Ú œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Ð œô¨Þœô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨çœô¨Øœô¨Úœô¨Ð œô¨Úœô¨Þœô¨Üœô¨Ðœô¨Ýœô¨Ô" );
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
				Log::instance().Trace( 10, "CTaskHandler::fnProcessThread: œô¨²œô¨Ýœô¨ãœô¨âœô¨àœô¨Õœô¨Ýœô¨Ýœô¨ï œô¨Þœô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð!" );


			pThis->m_mtxTasks.Lock();
				if( pThis->m_deqTasks.size() != 0 )
				{
					pTask = pThis->m_deqTasks.front();
					pThis->m_deqTasks.pop_front();
				}
			pThis->m_mtxTasks.Unlock();

			if( pTask.get() )
			{
				Log::instance().Trace( 10," CTaskHandler::fnProcessThread: œô¨²œô¨ëœô¨ßœô¨Þœô¨Ûœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï: %s", pTask->GetDescription().c_str() );
				pTask->Execute( pThis->m_CloseEv );
				pTask.Release();
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ: %s", e.what() );
	}/*catch( ... )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ" );
	}*/
/*	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: œô¨·œô¨Ðœô¨Òœô¨Õœô¨àœô¨èœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Ð œô¨Þœô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨çœô¨Øœô¨Úœô¨Ð œô¨Úœô¨Þœô¨Üœô¨Ðœô¨Ýœô¨Ô" );
	return 0;
}

*/
