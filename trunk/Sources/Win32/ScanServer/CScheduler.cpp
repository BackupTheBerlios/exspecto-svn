//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции планировщика
//-------------------------------------------------------------------------------------//
#include <iostream>
#include <tchar.h>
#include "CScheduler.h"
#include "windows.h"
#include "ServerSocket.h"

CScheduler::CScheduler(void)
{
	//Загружаем контейнер агентов
	m_mapAgentsContainer[ "127.0.0.1" ] = SmartPtr< CAgentHandler >( new CAgentHandler( "127.0.0.1" ) ); 
	
	Log::instance().Trace( 90, "CScheduler: создание, стартуем таймер" );
	m_pTrigger = std::auto_ptr< CTimer >( new CTimer( this ) );
	m_pTrigger->Start();
	Sleep(8000);
	m_pTrigger->Stop();
}

CScheduler::~CScheduler(void){}

void CScheduler::OnStartScan()
{
	std::vector< std::string > vecAdr;
	vecAdr.push_back( "127.0.0.1" );
	for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = m_mapAgentsContainer.begin(); It != m_mapAgentsContainer.end(); It++ )
	{
		enumAgentState bStatus;
		It->second->Open();
		if( It->second->IsOpened() )
		{
			It->second->GetStatus( bStatus );
			Log::instance().Trace( 10, "CScheduler: Статус агента: %d", bStatus );
		}
		It->second->Open();
		if( It->second->IsOpened() )
		{
			Log::instance().Trace( 10, "CScheduler: Агент вернул: %d", It->second->BeginScan( vecAdr ) );
		}
		It->second->Open();
		if( It->second->IsOpened() )
		{
			It->second->GetStatus( bStatus );
			Log::instance().Trace( 10, "CScheduler: Статус агента: %d", bStatus );
		}
		It->second->Open();
		if( It->second->IsOpened() )
		{
			Log::instance().Trace( 10, "CScheduler: Агент вернул: %d", It->second->StopScan() );
		}
		
	}
}

//Поток ожидания входящих соединений
unsigned _stdcall CScheduler::fnListenThreadProc(  void* pParameter )
{
	try{
		CScheduler* pThis = (CScheduler*)pParameter;
		CServerSocket sock;
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CScheduler:: Запуск потока ожидания входящих соединений" ); 
	    //связываем серверный сокет с локальным адресом
		sock.Bind( 3000, "127.0.0.1" );
		//переводим сокет в режим прослушивания
		sock.Listen();
		//Ожидаем входящее соединение и обрабатываем его
		while( ( NULL != ( client_sock = sock.Accept( adr ) ).get() ) && ( WAIT_OBJECT_0 != WaitForSingleObject( pThis->m_CloseEv, 0 ) ) )
		{
			Log::instance().Trace( 51, "CScheduler::ListenThread: Входящее соединение с адреса: %s", adr.strAddr.c_str() );
			try{
				//принимаем соединения только от заданного сервера сканирования
				if( m_mapAgentsContainer.find( adr.strAddr ) != m_mapAgentsContainer.end() ) 
					pThis->m_mapAgentsContainer[ adr.strAddr ]->OnConnection( client_sock );
				else
					Log::instance().Trace( 50, "CScheduler::ListenThread: Пришле пакет с адреса: %s. Игнорируем" );
			}catch( CSocket::SocketErr& e )
			{
				Log::instance().Trace( 50, "CScheduler::ListenThread: Исключение socket: %s", e.what() );
				//Если прислали пакет больше 10кб
				continue;
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CScheduler::ListenThread: Возникло исключение: %s", e.what() );
	}
	Log::instance().Trace( 50, "CScheduler::ListenThread: Завершение потока ожидания входящих сообщений" );
	return 0;
}
