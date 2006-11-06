 //------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
//-------------------------------------------------------------------------------------//
#include "CAgent.h"
#include "ServerSocket.h"


//Конструктор,strSchedulerAddress - адрес планировщика
CAgent::CAgent( std::string strSchedulerAddress ):m_CurState( Idling )
												 ,m_strSchedulerAddress( strSchedulerAddress )
{
	DWORD dwThreadId;
	//Запускаем поток прослушивания(ожидания входящих TCP соединений)
	::CloseHandle( ::CreateThread( 0, 0, fnListenThreadProc, this, 0, &dwThreadId ) );
	::InitializeCriticalSection( &m_csCurState );
	::InitializeCriticalSection( &m_csCommandExec );
	//Инициализируем карту обработчиков команд
	m_mapHandlers[ GET_STATUS ] = &CAgent::GetStatus;
	m_mapHandlers[ START_SCAN ] = &CAgent::StartScan;
	m_mapHandlers[ GET_DATA ] = &CAgent::GetData;
	m_mapHandlers[ STOP_SCAN ] = &CAgent::StopScan;
}

CAgent::~CAgent(void)
{
	::DeleteCriticalSection( &m_csCurState );
	::DeleteCriticalSection( &m_csCommandExec );
}

//Поток обработки входящих сообщений
DWORD WINAPI CAgent::fnProcessThreadProc( LPVOID pParameter )
{
	ProcessParam* pParams = (ProcessParam*)pParameter;
	BYTE bCommandId;
	CPacket Msg;
	try
	{
		//Задаем данные для разбора входящего пакета
		Msg.SetBuffer( pParams->pbBuf, pParams->iCount );
		for(;;)
		{
				if( !Msg.IsDone() )	
					Msg.GetCommandId( bCommandId );
				else
				{
					Log::instance().Trace( 95, "CAgent::fnProcessThreadProc: Обработка входящего пакета завершена" ); 
					break;
				}
			(pParams->pThis->*(pParams->pThis->m_mapHandlers[ bCommandId ]))( Msg, pParams->client_sock );
		}
		
		delete pParams->client_sock;
		delete pParams;
		return 0;
	}catch( CPacket::PacketFormatErr )
	{
		Log::instance().Trace( 50, "CAgent::fnProcessThreadProc: Пришел пакет неверного формата" );
		delete pParams->client_sock;
		delete pParams;
		return 0;
	}
	catch( std::exception& e )
	{
		Log::instance().Trace( 50, "CAgent::fnProcessThreadProc: Возникло исключение: %s", e.what() );
	}
	return 0;	
}

//Поток ожидания входящих соединений
DWORD WINAPI CAgent::fnListenThreadProc(  void* pParameter )
{
	try{
		CAgent* pThis = (CAgent*)pParameter;
		CServerSocket sock;
		CSocket* client_sock;
		BYTE pBuf[10240];
		int iCount = 0;
		DWORD dwThreadId;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CAgent:: Запуск потока ожидания входящих соединений" ); 
	    //связываем серверный сокет с локальным адресом
		sock.Bind( 5000, "127.0.0.1" );
		//переводим сокет в режим прослушивания
		sock.Listen();
		//Ожидаем входящее соединение и обрабатываем его
		while( NULL != ( client_sock = sock.Accept( adr ) ) )
		{
			Log::instance().Trace( 51, "CAgent::ListenThread: Входящее соединение с адреса: %s", adr.strAddr.c_str() );
			try{
				//принимаем соединения только от заданного сервера сканирования
				if( ( pThis->m_strSchedulerAddress == adr.strAddr ) && ( ( iCount = client_sock->Receive( pBuf, 10240 ) ) ) )
				{
					ProcessParam* params = new ProcessParam;
					params->client_sock = client_sock;
					params->iCount = iCount;
					params->pbBuf = pBuf;
					params->pThis = pThis;
					Log::instance().Dump( 90, pBuf, iCount, "CAgent::ListenThread: Обрабатываем пакет:" );
					::CloseHandle( ::CreateThread( 0, 0, fnProcessThreadProc, params, 0, &dwThreadId ) );
				}
			}catch( CSocket::SocketErr )
			{
				Log::instance().Trace( 50, "CAgent::ListenThread: Пришел пакет слишком большого размера" );
				//Если прислали пакет больше 10кб
				continue;
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: Возникло исключение: %s", e.what() );
	}
	return 0;
}

//Обработчики команд
enumAgentResponse CAgent::GetStatus( CPacket& Msg, CSocket* pSchedSocket )
{
	Log::instance().Trace( 90, "CAgent: Поступил запрос на получение статуса" );
	BYTE pbBuf[2];
	pbBuf[0] = 0xFF;
	pbBuf[1] = m_CurState;
	//отправляем текущее состояние
	pSchedSocket->Send( pbBuf, 2 );

	return RESP_OK;
}
	
enumAgentResponse CAgent::StartScan( CPacket& Msg, CSocket* pSchedSocket )
{
	Log::instance().Trace( 90, "CAgent: Поступил запрос на начало сканирования" );
	DWORD dwCount = 0;
	std::string strAddress;
	std::vector< std::string > List;
	
	::EnterCriticalSection( &m_csCommandExec );
	//Получаем кол-во адресов в пакете
	Msg.GetParam( dwCount );
	Log::instance().Trace( 80, "CAgent:StartScan: Кол-во адресов для сканирования: %d", dwCount );
	
	::EnterCriticalSection( &m_csCurState );
	m_CurState = Scanning;	
	::LeaveCriticalSection( &m_csCurState );

	//отправляем ответ серверу, команда принята на обработку
	enumAgentResponse resp = RESP_OK;
	pSchedSocket->Send( (BYTE*)&resp, 1 );
	for( unsigned int i = 0; i < dwCount; i++ )
	{
		//получаем очередной адрес и производим его сканирование по всем доступным протоколам
		Msg.GetAddress( strAddress );
		for( PluginIterator It = m_PluginContainer.begin(); It != m_PluginContainer.end(); It++ )
		{
			Log::instance().Trace( 80, "CAgent::StartScan: Сканируем адрес %s с помощью плагина %s", strAddress.c_str(), (*It)->GetProtocolName() );
			(*It)->Scan( strAddress, List );
		}
	}
	::EnterCriticalSection( &m_csCurState );
	m_CurState = Idling;		
	::LeaveCriticalSection( &m_csCurState );
	
	::LeaveCriticalSection( &m_csCommandExec );
	return RESP_OK;
}

enumAgentResponse CAgent::GetData( CPacket& Msg, CSocket* pSchedSocket )
{
	Log::instance().Trace( 90, "CAgent: Поступил запрос на получение данных" );
	return RESP_OK;
}
	
enumAgentResponse CAgent::StopScan( CPacket& Msg, CSocket* pSchedSocket )
{
	Log::instance().Trace( 90, "CAgent: Поступил запрос на окончание сканирования" );
	return RESP_OK;
}

