 //------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgent.h"
#include <process.h>
#include "ServerHandler.h"

//Описание типов параметров
static char* pAgentParamTypes[] = {
	SCHEDULER_ADDRESS, "string",
	LOG_LEVEL,	"int",
	EVENT_PORT, "int"
};
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------CAgent------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CAgent::CAgent()
{
	//Инициализация логера
	int iLogLevel;
	Settings::SetModule( "Agent", pAgentParamTypes, sizeof( pAgentParamTypes )/sizeof( pAgentParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );	

	Settings::instance().GetParam( SCHEDULER_ADDRESS, m_strSchedulerAddress );
	m_pMsgSock = SmartPtr< CServerSocket >( new CServerSocket() );
	m_pEventSock = SmartPtr< CClientSocket >( new CClientSocket() );
	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThreadProc, this, 0, NULL );
}

CAgent::~CAgent(void)
{
	try{
		m_CloseEvent.Set();
	
		m_pMsgSock->Close();
		Log::instance().Trace( 90, "CAgent::~CAgent: Ожидание закрытия потока прослушивания" );
		WaitForSingleObject( m_hListenThread, 10000 );
	
		CloseHandle( m_hListenThread );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::~CAgent: Возникло исключение: %s", e.what() );
	}catch(...)
	{
		Log::instance().Trace( 10," CAgent::ListenThread: Возникло неизвестное исключение" );
	}
}


//Поток ожидания входящих соединений
unsigned _stdcall CAgent::fnListenThreadProc(  void* pParameter )
{
	try{
		CAgent* pThis = (CAgent*)pParameter;
		
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;

		int iEventPort;
		Settings::instance().GetParam( EVENT_PORT, iEventPort );	
		Log::instance().Trace( 90, "CAgent:: Запуск потока ожидания входящих соединений" );
	    //связываем серверный сокет с локальным адресом
		pThis->m_pMsgSock->Bind( 5000, "127.0.0.1" );
		//переводим сокет в режим прослушивания
		pThis->m_pMsgSock->Listen();
		//Ожидаем входящее соединение и обрабатываем его
		while( NULL != ( client_sock = pThis->m_pMsgSock->Accept( adr ) ).get() )
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( pThis->m_CloseEvent, 0 ) )
				break;
			Log::instance().Trace( 51, "CAgent::ListenThread: Входящее соединение с адреса: %s", adr.strAddr.c_str() );
			//принимаем соединения только от заданного сервера сканирования
			if( pThis->m_strSchedulerAddress == adr.strAddr ) 
			{
				CServerHandler Handler( client_sock, pThis->m_pEventSock, pThis->m_strSchedulerAddress, iEventPort );
				pThis->m_vecConnections.push_back( SmartPtr< CConnectionHandler >( new CConnectionHandler( Handler ) ) );
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
	return 0;
}
