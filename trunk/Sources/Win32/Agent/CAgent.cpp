 //------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
//-------------------------------------------------------------------------------------//
#include "CAgent.h"
#include <process.h>
#include "SmartPtr.hpp"

enumAgentState CTask::m_CurState = Idling;
CCriticalSection CTask::m_csCurState;
CEvent CTask::m_CancelEv(false);
std::vector< std::string > CTask::m_vecData;
Container< CScanner*, PluginLoadStrategy > CStartScan::m_PluginContainer;

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------CAgent------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

//Конструктор,strSchedulerAddress - адрес планировщика
CAgent::CAgent()
{
	Settings::instance().GetParam( SCHEDULER_ADDRESS, m_strSchedulerAddress );
	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThreadProc, this, 0, NULL );
}

CAgent::~CAgent(void)
{
	try{
		m_CloseEvent.Set();
	
		m_sock.Close();
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
	
		Log::instance().Trace( 90, "CAgent:: Запуск потока ожидания входящих соединений" ); 
	    //связываем серверный сокет с локальным адресом
		pThis->m_sock.Bind( 5000, "127.0.0.1" );
		//переводим сокет в режим прослушивания
		pThis->m_sock.Listen();
		//Ожидаем входящее соединение и обрабатываем его
		while( NULL != ( client_sock = pThis->m_sock.Accept( adr ) ).get() )
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( pThis->m_CloseEvent, 0 ) )
				break;
			Log::instance().Trace( 51, "CAgent::ListenThread: Входящее соединение с адреса: %s", adr.strAddr.c_str() );
			//принимаем соединения только от заданного сервера сканирования
			if( pThis->m_strSchedulerAddress == adr.strAddr ) 
			{
				pThis->m_vecConnections.push_back( SmartPtr< CConnectionHandler >( new CConnectionHandler( client_sock ) ) );
			}else
				Log::instance().Trace( 50, "CAgent::ListenThread: Пришле пакет с адреса: %s. Игнорируем", adr.strAddr.c_str() );
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

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CConnectionHandler--------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CConnectionHandler::CConnectionHandler( SmartPtr< CSocket > pSocket ):m_pSocket( pSocket )
													 				 ,m_MessageParser( m_pSocket )
{
	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThread, this, 0, NULL );
}	
					
CConnectionHandler::~CConnectionHandler()
{
	try{
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: Закрытие соединения с %s", m_pSocket->GetRemoteHost().strAddr.c_str() );
		m_CloseEv.Set();
		m_pSocket->Close();
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: Ожидание закрытия потока прослушивания" );
		WaitForSingleObject( m_hListenThread, 10000 );
		CloseHandle( m_hListenThread );
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: Уничтожение" );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::~CAgent: Возникло исключение: %s", e.what() );
	}catch(...)
	{
		Log::instance().Trace( 10," CAgent::ListenThread: Возникло неизвестное исключение" );
	}		
}

unsigned _stdcall CConnectionHandler::fnListenThread( void* param )
{
	CConnectionHandler* pThis = (CConnectionHandler*)param;
	CPacket Msg;
	BYTE pBuf[10240];
	int iCount;

	try{
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: Запуск потока ожидания входящих сообщений c адреса %s", pThis->m_pSocket->GetRemoteHost().strAddr.c_str() );
		while( iCount = pThis->m_pSocket->Receive( pBuf, 10240 ) )
		{
			Log::instance().Dump( 90, pBuf, iCount, "CConnectionHandler::fnListenThread: Получен пакет: " );
			//Задаем данные для разбора входящего пакета
			Msg.SetBuffer( pBuf, iCount );
			std::vector< SmartPtr< CTask > > vecTasks = pThis->m_MessageParser.Parse( Msg );
			for( std::vector< SmartPtr< CTask > >::iterator It = vecTasks.begin(); It != vecTasks.end(); It++ )
				pThis->m_TaskHandler.AddTask( *It );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: Возникло исключение: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: Возникло неизвестное исключение" );
	}
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: Закрытие потока ожидания входящих сообщений с адреса %s", pThis->m_pSocket->GetRemoteHost().strAddr.c_str() );
	return 0;	
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CMessageParser------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
	
SmartPtr< CTask > CMessageParser::TaskFactory( BYTE bCommandId, CPacket& Msg )
{
	switch( bCommandId )
	{
		case GET_STATUS:
			return new CGetStatus( m_pSocket );
		case START_SCAN:
		{
			DWORD dwCount;
			std::string strAddress;
			std::vector< std::string > vecAddresses;
			//Получаем кол-во адресов в пакете
			Msg.GetParam( dwCount );
			for( unsigned int i = 0; i < dwCount; i++ )
			{
				//получаем очередной адрес
				Msg.GetAddress( strAddress );
				vecAddresses.push_back( strAddress );
			}	
			return new CStartScan( m_pSocket, vecAddresses );
		}
		case GET_DATA:
			return new CGetData( m_pSocket );
		case STOP_SCAN:
			return new CStopScan( m_pSocket );
		default:
			return NULL;
	}
}

std::vector< SmartPtr< CTask > > CMessageParser::Parse( CPacket& Message )
{
	Log::instance().Trace( 95, "CMessageParser::Parse: Обработка входящего пакета" );
	BYTE bCommandId;
	std::vector< SmartPtr< CTask > > vecRes;
	SmartPtr< CTask > pTask;
	for(;;)
	{
		if( !Message.IsDone() )	
			Message.GetCommandId( bCommandId );
		else
		{
			Log::instance().Trace( 95, "CMessageParser::Parse: Обработка входящего пакета завершена.Всего команд:%d", vecRes.size() ); 
			break;
		}
		if( NULL != ( pTask = TaskFactory( bCommandId, Message ) ).get() )
			vecRes.push_back( pTask );
	}
	return vecRes;
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CTaskHandler--------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CTaskHandler::CTaskHandler()
{
	m_hProcessThread = (HANDLE)::_beginthreadex( 0, 0, fnProcessThread, this, 0, NULL );
}

CTaskHandler::~CTaskHandler()
{
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: Закрытие обработчика заданий" );
	m_CloseEv.Set();
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: Ожидание закрытия потока обработки" );
	WaitForSingleObject( m_hProcessThread, 10000 );
	CloseHandle( m_hProcessThread );
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: Закрытие обработчика команд" );
}
	
void CTaskHandler::AddTask( SmartPtr< CTask > pTask )
{
	Log::instance().Trace( 95, "CTaskHandler::AddTask: Добавление задания: %s", pTask->GetDescription().c_str() );
	m_csTasks.Enter();
		if( !pTask->Immidiate() )
		{
			Log::instance().Trace( 95, "CTaskHandler::AddTask: Задание поставленно в очередь выполнения" );
			m_deqTasks.push_back( pTask );
		}
	m_csTasks.Leave();
}

unsigned _stdcall CTaskHandler::fnProcessThread( void* param )
{
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: Запуск потока обработчика команд" );
	CTaskHandler* pThis = (CTaskHandler*)param;
	SmartPtr< CTask > pTask;
	try{
		for(;;)
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( pThis->m_CloseEv, 0 ) )
				break;
			
			pThis->m_csTasks.Enter();
				if( pThis->m_deqTasks.size() != 0 )
				{
					pTask = pThis->m_deqTasks.front();
					pThis->m_deqTasks.pop_front();
				}
			pThis->m_csTasks.Leave();
			
			if( pTask.get() )
			{			
				Log::instance().Trace( 10," CTaskHandler::fnProcessThread: Выполнение задания: %s", pTask->GetDescription().c_str() );
				pTask->Execute();
				pTask.Release();
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: Возникло исключение: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: Возникло неизвестное исключение" );
	}	
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: Завершение потока обработчика команд" );
	return 0;
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CTask---------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

bool CGetStatus::Immidiate()
{
	m_csCurState.Enter();
		BYTE bResp[] = { RESP_OK, m_CurState }; 
		m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
		Log::instance().Dump( 90, bResp, sizeof( bResp ), "CGetStatus:Immidiate: Отправлен ответ:" );
	m_csCurState.Leave();
	return true;
}
	
bool CStartScan::Immidiate()
{
	BYTE bResp[] = { RESP_OK };
	m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
	Log::instance().Dump( 90, bResp, sizeof( bResp ), "CStartScan:Immidiate: Отправлен ответ:" );
	return false;
}
	
void CStartScan::Execute()
{
	m_vecData.clear();
	Log::instance().Trace( 90, "CStartScan: Поступил запрос на начало сканирования" );
	m_csCurState.Enter();
		m_CurState = Scanning;	
	m_csCurState.Leave();

	for( std::vector< std::string >::iterator AddrIt = m_vecAddresses.begin(); AddrIt != m_vecAddresses.end(); AddrIt++ )
	{
		for( PluginIterator PlugIt = m_PluginContainer.begin(); PlugIt != m_PluginContainer.end(); PlugIt++ )
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( m_CancelEv, 0 ) )
				break;
			Log::instance().Trace( 80, "CStartScan: Сканируем адрес %s с помощью плагина %s", AddrIt->c_str(), (*PlugIt)->GetProtocolName() );
			(*PlugIt)->Scan( *AddrIt, m_vecData, m_CancelEv );
		}
		if( WAIT_OBJECT_0 == WaitForSingleObject( m_CancelEv, 0 ) )
		{
			Log::instance().Trace( 90, "CStartScan: Сканирование отменено" );
			//Сбрасываем событие отмены
			m_CancelEv.Reset();
			break;
		}
	}
	//TODO:Уточнить,возможно следует использовать другой механизм блокировки,либо спин блокировку 
	m_csCurState.Enter();
		m_CurState = Idling;		
	m_csCurState.Leave();
}

bool CStopScan::Immidiate()
{
	Log::instance().Trace( 90, "CStopScan: Поступил запрос на отмену сканирования" );
	m_csCurState.Enter();
		if( Scanning == m_CurState )
		{
			Log::instance().Trace( 90, "CStopScan: Отменяем текущее сканирование" );
			Cancel();
		}else
			Log::instance().Trace( 90, "CStopScan: В данный момент не находится в состоянии сканирования" );
	m_csCurState.Leave();
	BYTE bResp[] = { RESP_OK };	
	m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
	Log::instance().Dump( 90, bResp, sizeof( bResp ), "CStopScan:Immidiate: Отправлен ответ:" );
	return true;
}

bool CGetData::Immidiate()
{
	Log::instance().Trace( 90, "CGetData: Поступил запрос на получение данных" );
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
	m_pSocket->Send( pbBuf.get(), iSize );
	Log::instance().Dump( 90, pbBuf.get(), iSize, "CGetData:Immidiate: Отправлен ответ:" );
	return true;
}
