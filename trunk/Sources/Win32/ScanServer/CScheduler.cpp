//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции планировщика
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CScheduler.h"
#include "constants.h"
#include "DbProviderFactory.h"

//Описание типов параметров
static char* pServerParamTypes[] = {
	TIMER_VALUE, "int",
	LOG_LEVEL,	"int",
	AGENT_LIST, "string-list",
	SCAN_AREA, "ip-list",
	DB_PROV_NAME, "string",
	EVENT_PORT, "int",
	AGENT_LISTEN_PORT, "int",
	POLLING_INTERVAL, "int"
};

CScheduler::CScheduler(void):m_bStarted(false)
{
	int iLogLevel;
	Settings::instance().SetModule( "ScanServer", pServerParamTypes, sizeof( pServerParamTypes )/sizeof( pServerParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );
	DbProviderFactory::instance();


	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThreadProc, this, 0, NULL );
	//Если поток не закрылся в течении 2 с - инициализация прошла успешно
	if( WAIT_TIMEOUT == WaitForSingleObject( m_hListenThread, 2000 ) )
		m_bStarted = true;

	Log::instance().Trace( 90, "CScheduler: создание, стартуем таймер" );
	m_pTrigger = std::auto_ptr< CTimer >( new CTimer( this ) );
	m_pTrigger->Start();
}

CScheduler::~CScheduler(void)
{
	m_CloseEv.Set();
	m_pTrigger->Stop();
	if( m_EventSock.IsConnected() )
	{
		Log::instance().Trace( 90, "CScheduler::~CScheduler: Канал событий поднят" );
		m_EventSock.Close();
	}
	else
		Log::instance().Trace( 90, "CScheduler::~CScheduler: Канал событий опущен" );

	Log::instance().Trace( 90, "CScheduler::~CScheduler: Ожидание закрытия потока прослушивания" );
	HANDLE hEvents[] = { m_hListenThread };
	WaitForMultipleObjects( sizeof( hEvents )/sizeof( hEvents[0] ), hEvents, TRUE, 10000 );
	CloseHandle( m_hListenThread );
}

void CScheduler::OnStartScan()
{
	try{
		//Засекаем время начала сканирования
		DWORD dwStartScanTime = GetTickCount();		
		m_csAgentsContainer.Enter();
		//Загружаем контейнер агентов
		m_mapAgentsContainer.clear();
		std::list< std::string > listAgents;
		Settings::instance().GetParam( "AgentList", listAgents );
		for( std::list< std::string >::iterator It = listAgents.begin(); It != listAgents.end(); It++ )
			m_mapAgentsContainer[ *It ] = SmartPtr< CAgentHandler >( new CAgentHandler( *It ) ); 
		m_csAgentsContainer.Leave();
		//Открываем соединение с агентами, отсеиваем недоступных
		for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = m_mapAgentsContainer.begin(); It != m_mapAgentsContainer.end();)
		{
            It->second->Open();
			if( !It->second->IsOpened() )
			{
				Log::instance().Trace( 10, "CScheduler::OnStartScan: Агент %s не доступен, исключаем из списка в текущем сканировании", It->first.c_str() );
				m_csAgentsContainer.Enter();
				m_mapAgentsContainer.erase( It++ );
				m_csAgentsContainer.Leave();
			}else
				It++;
		}
		if( m_mapAgentsContainer.empty() )
		{
			Log::instance().Trace( 10, "CScheduler::OnStartScan: Нет доступных агентов, отменяем сканирование" );
			return;
		}
		//Распределяем задания
		std::vector< std::string > vecAdr;
		Settings::instance().GetParam( SCAN_AREA, vecAdr );
		Log::instance().Trace( 12, "CScheduler::OnStartScan: Всего адресов для сканирования: %d", vecAdr.size() );
		std::map< std::string, std::vector< std::string > > mapRanges;
		int i = 1, iEndPos = 0, iStartPos = 0;
		std::vector< std::string > vecRange;
		for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = m_mapAgentsContainer.begin(); It != m_mapAgentsContainer.end(); It++, i++ )
		{
			iStartPos = iEndPos;
			iEndPos = i*(int)vecAdr.size()/(int)m_mapAgentsContainer.size();
			vecRange.insert( vecRange.end(), vecAdr.begin() + iStartPos, vecAdr.begin() + iEndPos );
			mapRanges[ It->first ] = vecRange;
			It->second->BeginScan( vecRange );
			vecRange.clear();
		}

		DWORD dwWaitRes;
		HANDLE hEvents[2];
		hEvents[0] = m_CloseEv;
		bool bFail = true;
		while( bFail )
		{
			//Ожидаем в течении периода опроса окончания сканирования
			for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItWait = m_mapAgentsContainer.begin(); ItWait != m_mapAgentsContainer.end(); ItWait++ )
			{
				int iPollingInterval;
				Settings::instance().GetParam( POLLING_INTERVAL, iPollingInterval );
				hEvents[1] = ItWait->second->GetScanFinishedEvent();
				if( (WAIT_OBJECT_0) == ( dwWaitRes = WaitForMultipleObjects( 2, hEvents, FALSE, iPollingInterval*1000 ) ) )
				{
					Log::instance().Trace( 10, "CScheduler::OnStartScan: Сканирование отменено!" );
					return;
				}else if( (WAIT_OBJECT_0+1) == dwWaitRes )
				{
					//Log::instance().Trace( 50, "CScheduler::OnStartScan: Агент %s закончил сканирование", ItWait->first.c_str() );
				}else if( WAIT_TIMEOUT == dwWaitRes )
				{
					break;
				}
			}
			//Если не дождались ответа от какого либо агента - распределяем 
			//его задание между уже закончившими сканирование
			if( WAIT_TIMEOUT == dwWaitRes )
			{
				//Агенты, закончившие сканирование
				std::vector< std::string > vecFinished;
				//Недоступные агенты
				std::vector< std::string > vecInaccess;
				//Опрашиваем агентов
				for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItPoll = m_mapAgentsContainer.begin(); ItPoll != m_mapAgentsContainer.end();)
				{
					enumAgentResponse Response;
					enumAgentState Status;
					try{
						Response = ItPoll->second->GetStatus( Status );
					}catch( SocketErr& )
					{
						//Приравниваем ошибку связи к ошибке обработки команды
						Response = RESP_PROC_ERR;
					}
					if( ( Response == RESP_OK ) && ( Status == Idling ) )
					{
						vecFinished.push_back( ItPoll->first );
						ItPoll++;
					}else if( Response != RESP_OK )
					{
						vecInaccess.push_back( ItPoll->first );
						m_csAgentsContainer.Enter();
						m_mapAgentsContainer.erase( ItPoll++ );
						m_csAgentsContainer.Leave();
					}else
						ItPoll++;
				}
				if( m_mapAgentsContainer.empty() )
				{
					Log::instance().Trace( 10, "CScheduler::OnStartScan: Доступных агентов не осталось,завершаем текущее сканирование" );
					break;
				}
				//Если не дождались ответа от какого либо агента - распределяем 
				//его задание между уже закончившими сканирование
				for( std::vector< std::string >::iterator ItInacc = vecInaccess.begin(); ItInacc != vecInaccess.end(); ItInacc++ )
				{
					Log::instance().Trace( 10, "CScheduler::OnStartScan: Агент %s не доступен, исключаем из списка в текущем сканировании", ItInacc->c_str() );
					if( !vecFinished.empty() )
					{
						Log::instance().Trace( 50, "CScheduler::OnStartScan: Разделяем задание между агентами,закончившими сканирование" );
						int iPartsCount = mapRanges[ *ItInacc ].size()/vecFinished.size();
						int i = 1, iStartPos = 0, iEndPos = 0;
						for( std::vector< std::string >::iterator ItFinished = vecFinished.begin(); ItFinished != vecFinished.end(); ItFinished++, i++ )
						{
							Log::instance().Trace( 50, "CScheduler::OnStartScan: Добавляем задание агенту %s", ItFinished->c_str() );
							iStartPos = iEndPos;
							iEndPos = i*iPartsCount;
							mapRanges[ *ItFinished ].insert( mapRanges[ *ItFinished ].begin(), mapRanges[ *ItInacc ].begin() + iStartPos, mapRanges[ *ItInacc ].begin() + iEndPos );
							m_mapAgentsContainer[ *ItFinished ]->BeginScan( mapRanges[ *ItFinished ] );
						}
						vecFinished.clear();
					}else
					{
						Log::instance().Trace( 50, "CScheduler::OnStartScan: Разделяем задание между оставшимися агентами" );
						int iPartsCount = mapRanges[ *ItInacc ].size()/m_mapAgentsContainer.size();
						int i = 1, iStartPos = 0, iEndPos = 0;
						for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItFinished = m_mapAgentsContainer.begin(); ItFinished != m_mapAgentsContainer.end(); ItFinished++, i++ )
						{
							Log::instance().Trace( 50, "CScheduler::OnStartScan: Добавляем задание агенту %s", ItFinished->first.c_str() );
							iStartPos = iEndPos;
							iEndPos = i*iPartsCount;
							mapRanges[ ItFinished->first ].insert( mapRanges[ ItFinished->first ].begin(), mapRanges[ ItFinished->first ].begin() + iStartPos, mapRanges[ ItFinished->first ].begin() + iEndPos );
							ItFinished->second->BeginScan( mapRanges[ ItFinished->first ] );
						}
					}
				}
			}else
				bFail = false;
		}
		Log::instance().Trace( 10, "CScheduler::OnStartScan: Сканирование закончено успешно" );
		Log::instance().Trace( 12, "CScheduler::OnStartScan: Общее время сканирования: %d", GetTickCount() - dwStartScanTime );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10, "CScheduler::OnStartScan: e= %s", e.what() );
	}
	catch( ... )
	{
		Log::instance().Trace( 10," CScheduler::OnStartScan: Возникло неизвестное исключение" );
	}
}
//Поток ожидания входящих соединений
unsigned _stdcall CScheduler::fnListenThreadProc(  void* pParameter )
{
	try{
		CScheduler* pThis = (CScheduler*)pParameter;
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CScheduler::fnListenThreadProc: Запуск потока ожидания входящих соединений" ); 
	    //связываем серверный сокет с локальным адресом
		int iEventPort;
		Settings::instance().GetParam( EVENT_PORT, iEventPort );
		pThis->m_EventSock.Bind( iEventPort );
		//переводим сокет в режим прослушивания
		pThis->m_EventSock.Listen();
		//Ожидаем входящее соединение и обрабатываем его
		while( ( NULL != ( client_sock = pThis->m_EventSock.Accept( adr ) ).get() ) && ( WAIT_OBJECT_0 != WaitForSingleObject( pThis->m_CloseEv, 0 ) ) )
		{
			CLock lock( pThis->m_csAgentsContainer );
			Log::instance().Trace( 51, "CScheduler::ListenThread: Входящее соединение с адреса: %s", adr.strAddr.c_str() );
			try{
				//принимаем соединения только от заданного сервера сканирования
				if( pThis->m_mapAgentsContainer.find( adr.strAddr ) != pThis->m_mapAgentsContainer.end() ) 
					pThis->m_mapAgentsContainer[ adr.strAddr ]->OnConnection( client_sock );
				else
					Log::instance().Trace( 50, "CScheduler::ListenThread: Пришле пакет с адреса: %s. Игнорируем" );
			}catch( SocketErr& e )
			{
				Log::instance().Trace( 50, "CScheduler::ListenThread: Исключение socket: %s", e.what() );
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

