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
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: �������М��ڜ������������؜��� ���ޜ��ќ������М��ќ��ޜ��������؜��ڜ��� ���ڜ��ޜ��ܜ��М��ݜ���" );
}

void CTaskHandler::AddTask( SmartPtr< CTask > pTask )
{
	Log::instance().Trace( 95, "CTaskHandler::AddTask: �������ޜ��ќ��М��Ҝ��ۜ��՜��ݜ��؜��� ���ל��М��Ԝ��М��ݜ��؜���: %s", pTask->GetDescription().c_str() );
	CLock lock( m_mtxTasks );
	if( !pTask->Immidiate() )
	{
		Log::instance().Trace( 95, "CTaskHandler::AddTask: �������М��Ԝ��М��ݜ��؜��� ���ߜ��ޜ��������М��Ҝ��ۜ��՜��ݜ��ݜ��� ���� ���ޜ�����՜������՜��Ԝ��� ���Ҝ�����ߜ��ޜ��ۜ��ݜ��՜��ݜ��؜���" );
		m_deqTasks.push_back( pTask );
		m_TaskAddedEv.Set();
	}
}

void CTaskHandler::CProcessThreadTask::Execute( CEvent& CancelEv )
{
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: �������М��ߜ��������� ���ߜ��ޜ�����ޜ��ڜ��� ���ޜ��ќ������М��ќ��ޜ��������؜��ڜ��� ���ڜ��ޜ��ܜ��М��ݜ���" );
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
				Log::instance().Trace( 10," CTaskHandler::fnProcessThread: ����������ߜ��ޜ��ۜ��ݜ��՜��ݜ��؜��� ���ל��М��Ԝ��М��ݜ��؜���: %s", pTask->GetDescription().c_str() );
				pTask->Execute( CancelEv );
				pTask.Release();
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���: %s", e.what() );
	}/*catch( ... )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���" );
	}*/
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: �������М��Ҝ��՜���������՜��ݜ��؜��� ���ߜ��ޜ�����ޜ��ڜ��� ���ޜ��ќ������М��ќ��ޜ��������؜��ڜ��� ���ڜ��ޜ��ܜ��М��ݜ���" );
}
/*
unsigned _stdcall CTaskHandler::fnProcessThread( void* param )
{
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: �������М��ߜ��������� ���ߜ��ޜ�����ޜ��ڜ��� ���ޜ��ќ������М��ќ��ޜ��������؜��ڜ��� ���ڜ��ޜ��ܜ��М��ݜ���" );
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
				Log::instance().Trace( 10, "CTaskHandler::fnProcessThread: �������ݜ������������՜��ݜ��ݜ��� ���ޜ�����؜��ќ��ڜ���!" );


			pThis->m_mtxTasks.Lock();
				if( pThis->m_deqTasks.size() != 0 )
				{
					pTask = pThis->m_deqTasks.front();
					pThis->m_deqTasks.pop_front();
				}
			pThis->m_mtxTasks.Unlock();

			if( pTask.get() )
			{
				Log::instance().Trace( 10," CTaskHandler::fnProcessThread: ����������ߜ��ޜ��ۜ��ݜ��՜��ݜ��؜��� ���ל��М��Ԝ��М��ݜ��؜���: %s", pTask->GetDescription().c_str() );
				pTask->Execute( pThis->m_CloseEv );
				pTask.Release();
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���: %s", e.what() );
	}/*catch( ... )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���" );
	}*/
/*	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: �������М��Ҝ��՜���������՜��ݜ��؜��� ���ߜ��ޜ�����ޜ��ڜ��� ���ޜ��ќ������М��ќ��ޜ��������؜��ڜ��� ���ڜ��ޜ��ܜ��М��ݜ���" );
	return 0;
}

*/
