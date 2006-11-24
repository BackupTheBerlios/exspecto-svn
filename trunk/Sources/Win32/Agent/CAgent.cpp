 //------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
//-------------------------------------------------------------------------------------//
#include "CAgent.h"
#include "ServerSocket.h"


//Конструктор,strSchedulerAddress - адрес планировщика
CAgent::CAgent():m_CurState( Idling )
{
	Settings::instance().GetParam( SCHEDULER_ADDRESS, m_strSchedulerAddress ); 
	DWORD dwThreadId;
	//Запускаем поток прослушивания(ожидания входящих TCP соединений)
	::CloseHandle( ::CreateThread( 0, 0, fnListenThreadProc, this, 0, &dwThreadId ) );
	::InitializeCriticalSection( &m_csCurState );
	::InitializeCriticalSection( &m_csCommandExec );
	::InitializeCriticalSection( &m_csCloseHandles );
	//Инициализируем событие
	m_hCancelEvent = CreateEvent( 0, 1, 0, 0 );
	//Инициализируем карту обработчиков команд
	m_mapHandlers[ GET_STATUS ] = &CAgent::GetStatus;
	m_mapHandlers[ START_SCAN ] = &CAgent::StartScan;
	m_mapHandlers[ GET_DATA ] = &CAgent::GetData;
	m_mapHandlers[ STOP_SCAN ] = &CAgent::StopScan;
}

CAgent::~CAgent(void)
{
	HANDLE hProcessThreads[ m_setProcessThreads.size() + 1 ];
	int i = 0;
	for( std::set< HANDLE >::iterator It = m_setProcessThreads.begin(); It != m_setProcessThreads.end(); It++, i++ )
		hProcessThreads[i] = *It;
	//Отменяем последний запрос
	if( m_CurState == Scanning )
		SetEvent( m_hCancelEvent );
	//TODO:ждать закрытия ListenThread
	//hProcessThreads[ i + 1 ] = 
	//Ждем максимум 10 сек пока все рабочие потоки не закроются и принудительно завершаем работу
	WaitForMultipleObjects( i + 1, hProcessThreads, TRUE, 10000 ); 
	::DeleteCriticalSection( &m_csCurState );
	::DeleteCriticalSection( &m_csCommandExec );
	::DeleteCriticalSection( &m_csCloseHandles );
	CloseHandle( m_hCancelEvent );
	
}

//Поток обработки входящих сообщений
DWORD WINAPI CAgent::fnProcessThreadProc( LPVOID pParameter )
{
	std::auto_ptr< ProcessParam > pParams( (ProcessParam*)pParameter );
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
			(pParams->pThis->*(pParams->pThis->m_mapHandlers[ bCommandId ]))( Msg, pParams->client_sock.get(), pParams->hCancelEvent );
		}
		//Удаляем себя из списка запущенных потоков обработки команд
		pParams->pThis->m_setProcessThreads.erase( GetCurrentThread() );
		//Записываем хэндл в список подлежащих закрытию
		::EnterCriticalSection( &pParams->pThis->m_csCloseHandles );
		pParams->pThis->m_vecCloseHandles.push_back( GetCurrentThread() );
		::LeaveCriticalSection( &pParams->pThis->m_csCloseHandles ); 
		return 0;
	}catch( CPacket::PacketFormatErr )
	{
		//Удаляем себя из списка запущенных потоков обработки команд
		pParams->pThis->m_setProcessThreads.erase( GetCurrentThread() );
		Log::instance().Trace( 50, "CAgent::fnProcessThreadProc: Пришел пакет неверного формата" );
		return 0;
	}
	catch( std::exception& e )
	{
		//Удаляем себя из списка запущенных потоков обработки команд
		pParams->pThis->m_setProcessThreads.erase( GetCurrentThread() );
		Log::instance().Trace( 50, "CAgent::fnProcessThreadProc: Возникло исключение: %s", e.what() );
		return 0;
	}
}

//Поток ожидания входящих соединений
DWORD WINAPI CAgent::fnListenThreadProc(  void* pParameter )
{
	try{
		CAgent* pThis = (CAgent*)pParameter;
		CServerSocket sock;
		std::auto_ptr< CSocket > client_sock;
		BYTE pBuf[10240];
		int iCount = 0;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CAgent:: Запуск потока ожидания входящих соединений" ); 
	    //связываем серверный сокет с локальным адресом
		sock.Bind( 5000, "172.16.6.53" );
		//переводим сокет в режим прослушивания
		sock.Listen();
		//Ожидаем входящее соединение и обрабатываем его
		while( ( NULL != ( client_sock = sock.Accept( adr ) ).get() ) && ( WAIT_OBJECT_0 != WaitForSingleObject( pThis->m_hCancelEvent, 0 ) ) )
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
					pThis->m_setProcessThreads.insert( ::CreateThread( 0, 0, fnProcessThreadProc, params, 0, NULL ) );
				}else
					Log::instance().Trace( 50, "CAgent::ListenThread: Пришле пакет с адреса: %s. Игнорируем" );
			}catch( CSocket::SocketErr )
			{
				Log::instance().Trace( 50, "CAgent::ListenThread: Пришел пакет слишком большого размера" );
				//Если прислали пакет больше 10кб
				continue;
			}
			//Закрываем все "отработавшие" дескрипторы
			::EnterCriticalSection( &pThis->m_csCloseHandles );
			for( std::vector< HANDLE >::iterator It = pThis->m_vecCloseHandles.begin(); It != pThis->m_vecCloseHandles.end(); It++ )
			{
				//Ждем максимум 10 сек если поток ещё не завершился
				if( WAIT_OBJECT_0 == WaitForSingleObject( *It, 10000 ) )
					CloseHandle( *It );
			}
			pThis->m_vecCloseHandles.clear();
			::LeaveCriticalSection( &pThis->m_csCloseHandles );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: Возникло исключение: %s", e.what() );
	}
	Log::instance().Trace( 50, "CAgent::ListenThread: Завершение потока ожидания входящих сообщений" );
	return 0;
}

//Обработчики команд
void CAgent::GetStatus( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent )
{
	Log::instance().Trace( 90, "CAgent: Поступил запрос на получение статуса" );
	//отправляем ответ серверу
	BYTE pbResp[2];
	pbResp[0] = (BYTE)RESP_OK;
	pbResp[1] = m_CurState;
	pSchedSocket->Send( pbResp, sizeof( pbResp ) );
}
	
void CAgent::StartScan( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent )
{
	Log::instance().Trace( 90, "CAgent: Поступил запрос на начало сканирования" );
	DWORD dwCount = 0;
	std::string strAddress;
	
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
	for( unsigned int i = 0; ( i < dwCount ) && ( WAIT_OBJECT_0 != WaitForSingleObject( hCancelEvent, 0 ) ); i++ )
	{
		//получаем очередной адрес и производим его сканирование по всем доступным протоколам
		Msg.GetAddress( strAddress );
		for( PluginIterator It = m_PluginContainer.begin(); ( It != m_PluginContainer.end() ) && ( WAIT_OBJECT_0 != WaitForSingleObject( hCancelEvent, 0 ) ); It++ )
		{
			Log::instance().Trace( 80, "CAgent::StartScan: Сканируем адрес %s с помощью плагина %s", strAddress.c_str(), (*It)->GetProtocolName() );
			(*It)->Scan( strAddress, m_vecData, hCancelEvent );
		}
	}
	//TODO:Уточнить,возможно следует использовать другой механизм блокировки,либо спин блокировку 
	::EnterCriticalSection( &m_csCurState );
	m_CurState = Idling;		
	::LeaveCriticalSection( &m_csCurState );
	
	::LeaveCriticalSection( &m_csCommandExec );
}

void CAgent::GetData( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent )
{
	Log::instance().Trace( 90, "CAgent: Поступил запрос на получение данных" );
	//Подсчитываем размер данных для отправки
	int iSize = 0;
	for( std::vector< std::string >::iterator It = m_vecData.begin(); It != m_vecData.end(); It++ )
	{
		//+1 - на завершающий ноль
		iSize += It->size() + 1;
	}
	//4 байта на размер 1 байт - результат обработки команды
	iSize += 4 + 1; 
	std::auto_ptr< BYTE > pbBuf = std::auto_ptr< BYTE >( new BYTE[ iSize ] );
	pbBuf.get()[0] = (BYTE)RESP_OK;
	::memcpy( pbBuf .get() + 1, (void*)&iSize, 4 );
	int iOffset = 5; 
	for( std::vector< std::string >::iterator It = m_vecData.begin(); It != m_vecData.end(); It++ )
	{
		strcpy( (char*)pbBuf.get() + iOffset, It->c_str() );
		iOffset += It->size() + 1;
	}
	pSchedSocket->Send( pbBuf.get(), iSize );	
}
	
void CAgent::StopScan( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent )
{
	Log::instance().Trace( 90, "CAgent::StopScan: Поступил запрос на окончание сканирования" );
	//отправляем ответ серверу, команда принята на обработку
	enumAgentResponse resp = RESP_OK;
	pSchedSocket->Send( (BYTE*)&resp, 1 );
	if( m_CurState == Scanning )
	{
		Log::instance().Trace( 90, "CAgent::StopScan: Останавливаем сканирование" ); 
		//Взводим событие отмены выполнения команды и ждем завершения потока обработки
		SetEvent( m_hCancelEvent );
	}
}

