 //------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgent.h"
#ifdef WIN32
#include <process.h>
#endif
#include "ServerHandler.h"

//Описание типов параметров
static char* pAgentParamTypes[] = {
	SCHEDULER_ADDRESS, "string",
	LOG_LEVEL,	"int",
	EVENT_PORT, "int",
	SCAN_THREADS_COUNT, "int",
	PING_ON, "bool",
	SCHEDULER_EVENT_PORT, "int",
	LISTEN_PORT, "int",
	RESOLVE_HOST, "bool"
};
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------CAgent------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CAgent::CAgent():m_bStarted( false ),m_ListenThread( SmartPtr<CThreadTask>( new CListenThreadTask( this ) ) )
{
	//Инициализация вспомогательных компонентов
	int iLogLevel;
	Settings::instance().SetModule( "Agent", pAgentParamTypes, sizeof( pAgentParamTypes )/sizeof( pAgentParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );

	Settings::instance().GetParam( SCHEDULER_ADDRESS, m_strSchedulerAddress );
	m_pMsgSock = SmartPtr< CServerSocket >( new CServerSocket() );

	//Если поток не закрылся в течении 2 с - инициализация прошла успешно
	Sleep(2000);
    m_bStarted = m_ListenThread.IsWorking();
}

CAgent::~CAgent(void)
{
	try{
		m_pMsgSock->Close();
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::~CAgent: Возникло исключение: %s", e.what() );
	}catch(...)
	{
		Log::instance().Trace( 10," CAgent::ListenThread: Возникло неизвестное исключение" );
	}
}

//Поток ожидания входящих соединений
void CAgent::CListenThreadTask::Execute( CEvent& CancelEv )
{
	try{
		SmartPtr< CSocket > client_sock;

		CServerSocket::structAddr adr;

		int iEventPort;
		Settings::instance().GetParam( SCHEDULER_EVENT_PORT, iEventPort );
		Log::instance().Trace( 90, "CAgent:: Запуск потока ожидания входящих соединений" );
	    //связываем серверный сокет с локальным адресом
		int iListenPort;
		Settings::instance().GetParam( LISTEN_PORT, iListenPort );
		m_pAgent->m_pMsgSock->Bind( iListenPort );
		//переводим сокет в режим прослушивания
		m_pAgent->m_pMsgSock->Listen();
		//Ожидаем входящее соединение и обрабатываем его
		while( NULL != ( client_sock = m_pAgent->m_pMsgSock->Accept( adr ) ).get() )
		{
		    if( CancelEv.TryWait() )
                break;
			Log::instance().Trace( 51, "CAgent::ListenThread: Входящее соединение с адреса: %s", adr.strAddr.c_str() );
			//принимаем соединения только от заданного сервера сканирования
			if( m_pAgent->m_strSchedulerAddress == adr.strAddr )
			{
				m_pAgent->m_pEventSock = SmartPtr< CClientSocket >( new CClientSocket() );
				CServerHandler Handler( client_sock, m_pAgent->m_pEventSock, m_pAgent->m_strSchedulerAddress, iEventPort );
				m_pAgent->m_vecConnections.push_back( SmartPtr< CConnectionHandler >( new CConnectionHandler( Handler ) ) );
			}else
				Log::instance().Trace( 50, "CAgent::ListenThread: Входящее соединение с адреса: %s. Игнорируем", adr.strAddr.c_str() );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: Возникло исключение: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: Возникло неизвестное исключение" );
	}
	Log::instance().Trace( 50, "CAgent::ListenThread: Завершение потока ожидания входящих сообщений" );
}

