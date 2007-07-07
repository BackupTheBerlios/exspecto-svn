//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgentHandler.h"
#include <process.h>
#include "DbProviderFactory.h"
#include <list>
#include <algorithm>



CAgentHandler::CAgentHandler( std::string strAgentAddress ):m_strAddress( strAgentAddress )
														   ,m_ScanFinished( false )
{
	m_pConnectionHandler = SmartPtr< CConnectionHandler >( new CConnectionHandler( this ) );
	//подготавливаем приемный буфер
	m_vecRecvBuf.resize( RECEIVE_BUF_START_SIZE );
}

CAgentHandler::~CAgentHandler()
{
	try
	{
		//Перед уничтожением закрываем соединение с агентом
		Close();
	}catch( ... ){}
}

//Отправить пакет Msg агенту и получить ответ в pbRespBuf, iRespSize - ожидаемый размер ответа
void CAgentHandler::SendMessage( COutPacket &Msg, CInPacket& Response )
{
	//Команды посылаются синхронно
	CLock lock( m_csExec );

	if( !IsOpened() )
	{
		Log::instance().Trace( 10, "CAgentHandler(%s)::SendMessage: Ошибка!Сессия не открыта!", m_strAddress.c_str() );
		throw HandlerErr( "Session has not been opened, but SendMessage called" );
	}
	Log::instance().Trace( 80, "CAgentHandler(%s)::SendMessage: отправляем пакет: %s", m_strAddress.c_str(), Msg.ToString().c_str() );
	m_Sock << Msg;

	int iCount;
	bool bEnd = false;
	std::vector< BYTE > vecPacketBuf;
	//Получаем ответ на сообщение
	while( !bEnd && ( iCount = m_Sock.Receive( &m_vecRecvBuf[0], (int)m_vecRecvBuf.size() ) ) > 0 )
	{
		Log::instance().Trace( 80, "CAgentHandler(%s)::SendMessage: iCount = %d", m_strAddress.c_str(), iCount );
		Log::instance().Dump( 200, (BYTE*)&m_vecRecvBuf[0], iCount, "CAgentHandler(%s)::SendMessage: Приняты данные:", m_strAddress.c_str() );
		//Проверяем на конец пакета
		if( ( iCount > (int)CInPacket::GetEndStamp().size() ) && ( 0 == memcmp( &CInPacket::GetEndStamp()[0], &m_vecRecvBuf[ iCount - CInPacket::GetEndStamp().size() ], CInPacket::GetEndStamp().size() ) ) )
		{
			iCount -= CInPacket::GetEndStamp().size();
			bEnd = true;
		//Если получили только маркер конца
		}else if ( ( iCount == CInPacket::GetEndStamp().size() ) && ( 0 == memcmp( &CInPacket::GetEndStamp()[0], &m_vecRecvBuf[ 0 ], CInPacket::GetEndStamp().size() ) ) )
			break;
		
		vecPacketBuf.insert( vecPacketBuf.end(), m_vecRecvBuf.begin(), m_vecRecvBuf.begin() + iCount );
		
		//Увеличиваем размер буфера при необходимости
		if( (iCount == ( (int)m_vecRecvBuf.size() ) ) )
			if( (m_vecRecvBuf.size()<<1) <= RECEIVE_BUF_MAX_SIZE )
				m_vecRecvBuf.resize( m_vecRecvBuf.size()<<1 );
			else if( m_vecRecvBuf.size() < RECEIVE_BUF_MAX_SIZE )
				m_vecRecvBuf.resize( RECEIVE_BUF_MAX_SIZE );
		Log::instance().Trace( 80, "CAgentHandler(%s)::SendMessage: Размер приемного буфера: %d", m_strAddress.c_str(), m_vecRecvBuf.size() );
	}
	if( 0 == iCount )
		throw HandlerErr( "Connection closed" );
	Response.Load( &vecPacketBuf[0], vecPacketBuf.size() );
}

void CAgentHandler::Open()
{
	if( m_Sock.IsConnected() )
		return;
	try
	{
		int iAgentListenPort;
		Settings::instance().GetParam( AGENT_LISTEN_PORT, iAgentListenPort );
		Log::instance().Trace( 90, "CAgentHandler::Open: открытие" );
		m_Sock.Connect( m_strAddress, iAgentListenPort );
	}catch( SocketErr& e )
	{
		Log::instance().Trace( 50, "CAgentHandler(%s)::Open: Ошибка соединения с агентом: %s; Описание ошибки: %s", m_strAddress.c_str(), m_strAddress.c_str(), e.what() );
	}
}

void CAgentHandler::Close()
{
	try
	{
		Log::instance().Trace( 90, "CAgentHandler(%s)::Close: закрытие", m_strAddress.c_str() );
		if( !m_Sock.IsConnected() )
			return;
		m_Sock.Close();
	}catch( SocketErr& e )
	{
		Log::instance().Trace( 50, "CAgentHandler(%s)::Close: ошибка закрытия соединения %s", m_strAddress.c_str(), e.what() );
	}
}

bool CAgentHandler::IsOpened()const
{
	return m_Sock.IsConnected();
}

void CAgentHandler::OnConnection( SmartPtr< CSocket > pSocket )
{
	Log::instance().Trace( 90, "CAgentHandler(%s)::OnConnection:..", m_strAddress.c_str() );
	m_pConnectionHandler->Listen( pSocket );
}

void CAgentHandler::OnMessage( CInPacket& Msg )
{
	try{
		std::string strEventId;
		Msg.GetField( EVENT_ID, strEventId );
		if( SCAN_COMPLETE == strEventId )
		{
			Log::instance().Trace( 90, "CAgentHandler(%s)::OnMessage: Сканирование закончено", m_strAddress.c_str() );
			GetData();
		}else
			Log::instance().Trace( 3, "CAgentHandler(%s)::OnMessage: Неизвестное событие", m_strAddress.c_str(), strEventId );
	}catch(std::exception& e)
	{
		Log::instance().Trace( 10, "CAgentHandler(%s)::OnMessage: Ошибка %s", m_strAddress.c_str() , e.what() );
	}
	catch(...)
	{
		Log::instance().Trace( 10, "CAgentHandler(%s)::OnMessage: Неизвестная ошибка ", m_strAddress.c_str() );
	}

}

std::string CAgentHandler::BeginScan( std::vector< std::string > vecAddresses )
{
	Log::instance().Trace( 90, "CAgentHandler(%s)::BeginScan: Отправка команды начала сканирования", m_strAddress.c_str() );
	COutPacket Msg;

	Msg.PutField( COMMAND_ID, START_SCAN );
	Log::instance().Trace( 90, "CAgentHandler(%s)::BeginScan: Всего адресов: %d", m_strAddress.c_str(), vecAddresses.size() );
	Msg.PutField( ADDR_COUNT, (int)vecAddresses.size() );
	for( std::vector< std::string >::iterator It = vecAddresses.begin(); It != vecAddresses.end(); It++ )
	{
		Log::instance().Trace( 92, "CAgentHandler(%s)::BeginScan: Добавляем адрес %s", m_strAddress.c_str(), It->c_str() );
		Msg.PutField( HOST_ADDR, *It );
	}
	Log::instance().Trace( 92, "CAgentHandler(%s)::BeginScan: Отправляем сообщение агенту", m_strAddress.c_str() );
	CInPacket Response;
	SendMessage( Msg, Response );
	std::string strStatus;
	Response.GetField( COMMAND_STAT, strStatus );
	m_ScanFinished.Reset();
	return strStatus; 
}
	
std::string CAgentHandler::StopScan()
{
	Log::instance().Trace( 90, "CAgentHandler(%s)::StopScan: Отправка команды окончания сканирования", m_strAddress.c_str() );
	COutPacket Msg;

	Msg.PutField( COMMAND_ID, STOP_SCAN );

	std::string strRes;
	CInPacket Response;
	SendMessage( Msg, Response );
	Response.GetField( COMMAND_STAT, strRes );
	return strRes; 
}
	
std::string CAgentHandler::GetStatus( std::string& Status )
{
	Log::instance().Trace( 90, "CAgentHandler(%s)::GetStatus: Отправка команды получения статуса", m_strAddress.c_str() );
	COutPacket Msg;
	Msg.PutField( COMMAND_ID, GET_STATUS );

	std::string strRes;
	CInPacket Response;
	SendMessage( Msg, Response );
	Response.GetField( COMMAND_STAT, strRes );
	if( AGENT_RESP_OK == strRes )
		Response.GetField( AGENT_STATUS, Status );
	return strRes;
}

std::string CAgentHandler::GetData()
{
	Log::instance().Trace( 90, "CAgentHandler::GetData(%s): Отправка команды получения данных", m_strAddress.c_str() );	
	COutPacket Msg;

	Msg.PutField( COMMAND_ID, GET_DATA );
	Msg.PutField( FILES_COUNT, 100 );

	//отправляем команду
	std::string strRes;
	CInPacket Response;
	bool bFilesLeft = true;
	DWORD dwTime = 0;
	while( bFilesLeft )
	{
		SendMessage( Msg, Response );
		Response.GetField( COMMAND_STAT, strRes );
		if( AGENT_RESP_OK != strRes )
			break;
		Response.GetField( FILES_LEFT, bFilesLeft );
		int iFilesCount;
		Response.GetField( FILES_COUNT, iFilesCount );
		if( iFilesCount > 0 )
		{
			std::set<std::string> setErased;
			hostRec TmpHost;
			Response.GetFirstHostRec( TmpHost );
			do
			{
				if( setErased.find( TmpHost.IPNum ) == setErased.end() )
				{
					DbProviderFactory::instance().GetProviderInstance()->EraseHost( TmpHost.HostName, TmpHost.IPNum, 0 );
					setErased.insert( TmpHost.IPNum );
				}
				DWORD dwtick1,dwtick2;
				dwtick1 = GetTickCount();
				DbProviderFactory::instance().GetProviderInstance()->AddFiles( TmpHost );
				dwtick2 = GetTickCount();
				dwTime += dwtick2 - dwtick1;
			}while( Response.GetNextHostRec( TmpHost ) );
		}
	}
	Log::instance().Trace( 0, "CAgentHandler::GetData(%s):Суммарное время записи в БД: %d", m_strAddress.c_str(), dwTime );
	m_ScanFinished.Set();
	return strRes;
}

void CAgentHandler::CConnectionHandler::Listen( SmartPtr<CSocket> pSocket )
{
	m_pSocket = pSocket;
	if( WAIT_OBJECT_0 != WaitForSingleObject( m_hListenThread, 0 ) )
		m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThread, this, 0, NULL );
}

unsigned __stdcall CAgentHandler::CConnectionHandler::fnListenThread( void* param )
{
	CConnectionHandler* pThis = (CConnectionHandler*)param;
	Log::instance().Trace( 90, "CConnectionHandler(%s)::fnListenThread: Запуск", pThis->m_pAgentHandler->m_strAddress.c_str() );
	try{
		BYTE pBuf[10240];
		int iCount;
		while( iCount = pThis->m_pSocket->Receive( pBuf, sizeof( pBuf ) ) )
		{
			Log::instance().Dump( 90, pBuf, iCount, "CConnectionHandler(%s)::fnListenThread: Получен пакет:", pThis->m_pAgentHandler->m_strAddress.c_str() );
			CInPacket Msg( pBuf, iCount );
			pThis->m_pAgentHandler->OnMessage( Msg );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CConnectionHandler(%s)::fnListenThread: Возникло исключение: %s", pThis->m_pAgentHandler->m_strAddress.c_str(), e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CConnectionHandler(%s)::fnListenThread: Возникло неизвестное исключение", pThis->m_pAgentHandler->m_strAddress.c_str() );
	}
	Log::instance().Trace( 90, "CConnectionHandler(%s)::fnListenThread: Закрытие", pThis->m_pAgentHandler->m_strAddress.c_str() );
	CloseHandle( pThis->m_hListenThread );
	return 0;
}

CAgentHandler::CConnectionHandler::~CConnectionHandler()
{
	//Если соединение событий существует - закрываем его
	if( NULL != m_pSocket.get() )
		m_pSocket->Close();
	SetEvent( m_hCloseEv );
	WaitForSingleObject( m_hListenThread, 10000 );
	CloseHandle( m_hCloseEv );
}
