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

	//Задаем данные для разбора входящего пакета
	Msg.SetBuffer( pParams->pbBuf, pParams->iCount );
	for(;;)
	{
		try{
			 Msg.GetCommandId( bCommandId );
		}catch( CPacket::PacketFormatErr )
		{
			Log::instance().Trace( 11, "CAgent::fnProcessThreadProc: Пришел пакет неверного формата" );
			break;
		}
		(pParams->pThis->*(pParams->pThis->m_mapHandlers[ bCommandId ]))( Msg, pParams->client_sock );
	}
	delete pParams->client_sock;
	delete pParams;
	return 0;
}

//Поток ожидания входящих соединений
DWORD WINAPI CAgent::fnListenThreadProc(  void* pParameter )
{
	CAgent* pThis = (CAgent*)pParameter;
	CServerSocket sock;
	CSocket* client_sock;
	BYTE pBuf[10240];
	int iCount = 0;
	DWORD dwThreadId;

	CServerSocket::structAddr adr;

    //связываем серверный сокет с локальным адресом
	sock.Bind( 5000, "127.0.0.1" );
	//переводим сокет в режим прослушивания
	sock.Listen();
	//Ожидаем входящее соединение и обрабатываем его
	while( NULL != ( client_sock = sock.Accept( adr ) ) )
	{
		Log::instance().Trace( 51, "CAgent::fnListenThreadProc: Входящее соединение с адреса: %s", adr.strAddr.c_str() );
		try{
			//принимаем соединения только от заданного сервера сканирования
			if( ( pThis->m_strSchedulerAddress == adr.strAddr ) && ( ( iCount = client_sock->Receive( pBuf, 10240 ) ) ) )
			{
				ProcessParam* params = new ProcessParam;
				params->client_sock = client_sock;
				params->iCount = iCount;
				params->pbBuf = pBuf;
				params->pThis = pThis;
				::CloseHandle( ::CreateThread( 0, 0, fnProcessThreadProc, params, 0, &dwThreadId ) );
			}
		}catch( CSocket::SocketErr )
		{
			//Если прислали пакет больше 10кб
			continue;
		}
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
	//закрываем соединение
	pSchedSocket->Close();
	
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
	
	std::vector< std::string >::iterator It;
	for( It = List.begin(); It != List.end(); It++ )
	{
		//std::cout << *It << std::endl;
	}
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

