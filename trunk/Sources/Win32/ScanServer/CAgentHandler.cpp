//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgentHandler.h"
#include <process.h>

#define RECEIVE_BUF_START_SIZE 255
#define RECEIVE_BUF_MAX_SIZE 50000000

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
void CAgentHandler::SendMessage( CPacket &Msg, std::vector< BYTE >& vecBuf )
{
	const static BYTE EndStamp[] = { 0, 0x10, 0x13, 0 };
	if( !IsOpened() )
	{
		Log::instance().Trace( 10, "CAgentHandler::SendMessage: Ошибка!Сессия не открыта!" );
		throw HandlerErr( "Session has not been opened, but SendMessage called" );
	}
	BYTE* pBuf = NULL;
	int iSize = 0;
	//Получаем буфер с данными сообщения Msg для отправки по сети
	Msg.GetBuffer( pBuf, iSize );
	Log::instance().Dump( 80, pBuf, iSize, "CAgentHandler::SendMessage: отправляем буфер:" );
	if( 0 == iSize )
		throw HandlerErr( "Attempt to send message with zero-length was made" );		
	m_Sock.Send( pBuf, iSize );

	int iCount;
	bool bEnd = false;
	//Получаем ответ на сообщение
	while( !bEnd && ( iCount = m_Sock.Receive( &m_vecRecvBuf[0], (int)m_vecRecvBuf.size() ) ) > 0 )
	{
		Log::instance().Trace( 80, "CAgentHandler::SendMessage: iCount = %d", iCount );
		if( ( iCount > 4 ) && ( 0 == memcmp( EndStamp, &m_vecRecvBuf[ iCount - 4 ], 4 ) ) )
		{
			iCount -= 4;
			bEnd = true;
		}
		vecBuf.insert( vecBuf.end(), m_vecRecvBuf.begin(), m_vecRecvBuf.begin() + iCount );
		if( (iCount == (int)m_vecRecvBuf.size() ) )
			if( (m_vecRecvBuf.size()<<1) <= RECEIVE_BUF_MAX_SIZE )
				m_vecRecvBuf.resize( m_vecRecvBuf.size()<<1 );
			else if( m_vecRecvBuf.size() < RECEIVE_BUF_MAX_SIZE )
				m_vecRecvBuf.resize( RECEIVE_BUF_MAX_SIZE );
		Log::instance().Trace( 80, "CAgentHandler::SendMessage: Размер приемного буфера: %d", m_vecRecvBuf.size() );
	}
	Log::instance().Trace( 80, "CAgentHandler::SendMessage: Размер данных %d:", vecBuf.size() );
	Log::instance().Dump( 80, &vecBuf[0], (int)vecBuf.size(), "CAgentHandler::SendMessage: Получили ответ:" );
	if( 0 == iCount )
		throw HandlerErr( "Connection closed" );
}

void CAgentHandler::Open()
{
	if( m_Sock.IsConnected() )
		return;
	try
	{
		Log::instance().Trace( 90, "CAgentHandler::Open: открытие" );
		m_Sock.Connect( m_strAddress, PORT );
	}catch( SocketErr& e )
	{
		Log::instance().Trace( 50, "CAgentHandler::Open: Ошибка соединения с агентом: %s; Описание ошибки: %s", m_strAddress.c_str(), e.what() );
	}
}

void CAgentHandler::Close()
{
	try
	{
		Log::instance().Trace( 90, "CAgentHandler::Close: закрытие" );
		if( !m_Sock.IsConnected() )
			return;
		m_Sock.Close();
	}catch( SocketErr& e )
	{
		Log::instance().Trace( 50, "CAgentHandler::Close: ошибка закрытия соединения %s", e.what() );
	}
}

bool CAgentHandler::IsOpened()const
{
	return m_Sock.IsConnected();
}

void CAgentHandler::OnConnection( SmartPtr< CSocket > pSocket )
{
	Log::instance().Trace( 90, "CAgentHandler::OnConnection:.." );
	m_pConnectionHandler->Listen( pSocket );
}

void CAgentHandler::OnMessage( CPacket& Msg )
{
	try{
		BYTE bCommandId;
		Msg.GetCommandId( bCommandId );
		switch( bCommandId )
		{
			case ScanComplete:
				Log::instance().Trace( 90, "CAgentHandler::OnMessage: Сканирование закончено" );
				GetData();
				m_ScanFinished.Set();
				break;
		};
	}catch(...)
	{
		//TODO:
	}
}

enumAgentResponse CAgentHandler::BeginScan( std::vector< std::string > vecAddresses )
{
	Log::instance().Trace( 90, "CAgentHandler::BeginScan: Отправка команды начала сканирования" );
	CPacket Msg;

	Msg.BeginCommand( START_SCAN );
	Log::instance().Trace( 90, "CAgentHandler::BeginScan: Всего адресов: %d", vecAddresses.size() );
	Msg.AddParam( (DWORD)vecAddresses.size() );
	for( std::vector< std::string >::iterator It = vecAddresses.begin(); It != vecAddresses.end(); It++ )
	{
		Log::instance().Trace( 92, "CAgentHandler::BeginScan: Добавляем адрес %s", It->c_str() );
		Msg.AddAddress( *It );
	}
	Msg.EndCommand();
	
	Log::instance().Trace( 92, "CAgentHandler::BeginScan: Отправляем сообщение агенту: %s", m_strAddress.c_str() );
	std::vector<BYTE> vecRes;
	SendMessage( Msg, vecRes );
	m_ScanFinished.Reset();
	return (enumAgentResponse)vecRes[0]; 
}
	
enumAgentResponse CAgentHandler::StopScan()
{
	Log::instance().Trace( 90, "CAgentHandler::StopScan: Отправка команды окончания сканирования" );
	CPacket Msg;

	Msg.BeginCommand( STOP_SCAN );
	Msg.EndCommand();

	SmartPtr< BYTE, AllocMalloc<BYTE> >  pbRecvBuf;
	std::vector<BYTE> vecRes;
	SendMessage( Msg, vecRes );
	return (enumAgentResponse)vecRes[0]; 
}
	
enumAgentResponse CAgentHandler::GetStatus( enumAgentState& Status )
{
	Log::instance().Trace( 90, "CAgentHandler::GetStatus: Отправка команды получения статуса" );
	CPacket Msg;

	Msg.BeginCommand( GET_STATUS );
	Msg.EndCommand();

	std::vector<BYTE> vecRes;
	SendMessage( Msg, vecRes );
	if( RESP_OK != vecRes[0] )
	{
		Log::instance().Trace( 50, "CAgentHandler::GetStatus: Команда получения статуса не выполнена, код возврата: %d", vecRes[0] );
		return (enumAgentResponse)vecRes[0];
	}
	Status = (enumAgentState)vecRes[1];
	Log::instance().Trace( 80, "CAgentHandler::GetStatus: Получен статус: %d", Status );
	return RESP_OK;
}
	
enumAgentResponse CAgentHandler::GetData()
{
	Log::instance().Trace( 90, "CAgentHandler::GetData: Отправка команды получения данных" );	
	CPacket Msg;

	Msg.BeginCommand( GET_DATA );
	Msg.EndCommand();

	//отправляем команду
	std::vector<BYTE> vecRes;
	SendMessage( Msg, vecRes );
	if( RESP_OK != vecRes[0] )
	{
		Log::instance().Trace( 50, "CAgentHandler::GetData: Команда получения данных не выполнена, код возврата: %d", vecRes[0] );
		return (enumAgentResponse)vecRes[0];
	}
	unsigned int iDataSize;
	::memcpy( (BYTE*)&iDataSize, &vecRes[1], 4 );
	Log::instance().Trace( 80, "CAgentHandler::GetData: Размер данных: %d", iDataSize );
	Log::instance().Dump( 90, &vecRes[1], iDataSize, "CAgentHandler::GetData: Получены данные:" );
	return RESP_OK;
}

void CConnectionHandler::Listen( SmartPtr<CSocket> pSocket )
{
	m_pSocket = pSocket;
	if( WAIT_OBJECT_0 != WaitForSingleObject( m_hListenThread, 0 ) )
		m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThread, this, 0, NULL );
}

unsigned __stdcall CConnectionHandler::fnListenThread( void* param )
{
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: Запуск" );
	CConnectionHandler* pThis = (CConnectionHandler*)param;
	try{
		CPacket Msg;
		BYTE pBuf[10240];
		int iCount;
		while( iCount = pThis->m_pSocket->Receive( pBuf, sizeof( pBuf ) ) )
		{
			Log::instance().Dump( 90, pBuf, iCount, "CConnectionHandler::fnListenThread: Получен пакет:" );
			Msg.SetBuffer( pBuf, iCount );
			pThis->m_pAgentHandler->OnMessage( Msg );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: Возникло исключение: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: Возникло неизвестное исключение" );
	}
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: Закрытие" );
	CloseHandle( pThis->m_hListenThread );
	return 0;
}

CConnectionHandler::~CConnectionHandler()
{
	SetEvent( m_hCloseEv );
	WaitForSingleObject( m_hListenThread, 10000 );
	CloseHandle( m_hCloseEv );
}
