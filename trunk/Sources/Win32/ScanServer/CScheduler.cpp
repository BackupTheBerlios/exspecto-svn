//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции планировщика
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CScheduler.h"
#include "constants.h"

//Описание типов параметров
static char* pServerParamTypes[] = {
	TIMER_VALUE, "int",
	LOG_LEVEL,	"int",
	AGENT_LIST, "string-list",
	SCAN_AREA, "ip-list"
};

CScheduler::CScheduler(void)
{
	int iLogLevel;
	Settings::instance().SetModule( "ScanServer", pServerParamTypes, sizeof( pServerParamTypes )/sizeof( pServerParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );
	
	std::list< std::string > listAgents;
	Settings::instance().GetParam( "AgentList", listAgents );
	//Загружаем контейнер агентов
	for( std::list< std::string >::iterator It = listAgents.begin(); It != listAgents.end(); It++ )
		m_mapAgentsContainer[ *It ] = SmartPtr< CAgentHandler >( new CAgentHandler( *It ) ); 
	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThreadProc, this, 0, NULL );
//	m_hMainThread = (HANDLE)_beginthreadex( 0, 0, fnMainThreadProc, this, 0, NULL );
		
	Log::instance().Trace( 90, "CScheduler: создание, стартуем таймер" );
	m_pTrigger = std::auto_ptr< CTimer >( new CTimer( this ) );
	m_pTrigger->Start();
}

CScheduler::~CScheduler(void)
{
	m_pTrigger->Stop();
	m_CloseEv.Set();
	if( m_EventSock.IsConnected() )
	{
		Log::instance().Trace( 90, "CScheduler::~CScheduler: Канал событий поднят" );
		m_EventSock.Close();
	}
	else
		Log::instance().Trace( 90, "CScheduler::~CScheduler: Канал событий опущен" );

	Log::instance().Trace( 90, "CScheduler::~CScheduler: Ожидание закрытия потока прослушивания" );
	HANDLE hEvents[] = { m_hMainThread, m_hListenThread };
	WaitForMultipleObjects( sizeof( hEvents )/sizeof( hEvents[0] ), hEvents, TRUE, 10000 );
	Log::instance().Trace( 90, "CScheduler::~CScheduler: 2" );
	CloseHandle( m_hListenThread );
}

void CScheduler::OnStartScan()
{
	try{
		std::vector< std::string > listAdr;
		Settings::instance().GetParam( SCAN_AREA, listAdr );

		std::map< std::string, SmartPtr< CAgentHandler > > mapAgents = m_mapAgentsContainer;
		std::vector< HANDLE > vecFinishEvents;
		std::vector< std::vector< std::string >::iterator > vecRanges;
        int i = 1, iEndPos = 0;
		bool bFail = true;
		while( bFail )
		{
			vecFinishEvents.clear();
			vecRanges.clear();
			vecRanges.push_back( listAdr.begin() );
			for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = mapAgents.begin(); It != mapAgents.end(); It++, i++ )
			{
				iEndPos = i*(int)listAdr.size()/(int)mapAgents.size();
				vecRanges.push_back( listAdr.begin() + iEndPos );

				It->second->Open();
				if( !It->second->IsOpened() )
				{
					Log::instance().Trace( 10, "CScheduler::OnStartScan: Агент %s не доступен, исключаем из списка в текущем сканировании", It->first.c_str() );
					mapAgents.erase( It );
					bFail = true;
					break;
				}
				bFail = false;
				vecFinishEvents.push_back( It->second->GetScanFinishedEvent() );
			}
			if( mapAgents.empty() )
			{
				Log::instance().Trace( 10, "CScheduler::OnStartScan: Нет доступных агентов, отменяем сканирование" );
				return;
			}
		}
		int j = 0;
		for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = mapAgents.begin(); It != mapAgents.end(); It++, j++ )
		{
			std::vector<std::string> vec( vecRanges[j], vecRanges[j+1] );
			It->second->BeginScan( vec );
		}
		DWORD dwWaitRes;
		if( WAIT_TIMEOUT == ( dwWaitRes = WaitForMultipleObjects( (DWORD)vecFinishEvents.size(), (HANDLE*)&vecFinishEvents[0], TRUE, 10*60*1000 ) ) )
		{
			//TODO:
		}else if( WAIT_FAILED == dwWaitRes );
			//TODO:
		else
		{
			Log::instance().Trace( 10, "CScheduler::OnStartScan: Сканирование закончено успешно" );
		}

	}catch( std::exception& e )
	{
		Log::instance().Trace( 10, "CScheduler::OnStartScan: e= %s", e.what() );
	}
	catch( ... )
	{
		Log::instance().Trace( 10," CScheduler::OnStartScan: Возникло неизвестное исключение" );
	}
}
/*
//Основной поток планировщика
unsigned _stdcall CScheduler::fnMainThreadProc(  void* pParameter )
{
	CScheduler* pThis = (CScheduler*)pParameter;

	return 0;
}
*/
//Поток ожидания входящих соединений
unsigned _stdcall CScheduler::fnListenThreadProc(  void* pParameter )
{
	try{
		CScheduler* pThis = (CScheduler*)pParameter;
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CScheduler::fnListenThreadProc: Запуск потока ожидания входящих соединений" ); 
	    //связываем серверный сокет с локальным адресом
		pThis->m_EventSock.Bind( 3000, "127.0.0.1" );
		//переводим сокет в режим прослушивания
		pThis->m_EventSock.Listen();
		//Ожидаем входящее соединение и обрабатываем его
		while( ( NULL != ( client_sock = pThis->m_EventSock.Accept( adr ) ).get() ) && ( WAIT_OBJECT_0 != WaitForSingleObject( pThis->m_CloseEv, 0 ) ) )
		{
			Log::instance().Trace( 51, "CScheduler::ListenThread: Входящее соединение с адреса: %s", adr.strAddr.c_str() );
			try{
				//принимаем соединения только от заданного сервера сканирования
				if( pThis->m_mapAgentsContainer.find( adr.strAddr ) != pThis->m_mapAgentsContainer.end() ) 
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
	}catch( ... )
	{
		Log::instance().Trace( 10," CScheduler::ListenThread: Возникло неизвестное исключение" );
	}
	Log::instance().Trace( 50, "CScheduler::ListenThread: Завершение потока ожидания входящих сообщений" );
	return 0;
}
