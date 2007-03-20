//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgentHandler.h"
#include <process.h>

CAgentHandler::CAgentHandler( std::string strAgentAddress ):m_strAddress( strAgentAddress )
														   ,m_bFinished( false )
{
	m_pConnectionHandler = SmartPtr< CConnectionHandler >( new CConnectionHandler( this ) );
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
SmartPtr< BYTE, AllocMalloc<BYTE> > CAgentHandler::SendMessage( CPacket &Msg )throw( HandlerErr, CSocket::SocketErr )
{
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

	BYTE pbBuf[255];
	int iCount;
	int iOffset = 0;
	SmartPtr< BYTE, AllocMalloc<BYTE> > pbData = SmartPtr< BYTE, AllocMalloc<BYTE> >( (BYTE*)malloc( sizeof(pbBuf) ) );
	//Получаем ответ на сообщение
	while( ( iCount = m_Sock.Receive( pbBuf, sizeof(pbBuf) ) ) == sizeof(pbBuf) ) 
	{
		Log::instance().Trace( 80, "CAgentHandler::SendMessage: iCount = %d", iCount );
		pbData.Realloc( iOffset + iCount + 10 );
		memcpy( pbData.get() + iOffset, pbBuf, iCount );
		iOffset += iCount; 
	}
	memcpy( pbData.get() + iOffset, pbBuf, iCount );
	Log::instance().Dump( 80, pbData.get(), iCount, "CAgentHandler::SendMessage: Получили ответ:" );
	if( 0 == iCount )
		throw HandlerErr( "Connection closed" );
	return pbData;
}

void CAgentHandler::Open()
{
	if( m_Sock.IsConnected() )
		return;
	try
	{
		Log::instance().Trace( 90, "CAgentHandler::Open: открытие" );
		m_Sock.Connect( m_strAddress, PORT );
	}catch( CSocket::SocketErr& e )
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
	}catch( CSocket::SocketErr& e )
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
	BYTE bCommandId;
	Msg.GetCommandId( bCommandId );
	switch( bCommandId )
	{
		case ScanComplete:
			Log::instance().Trace( 90, "CAgentHandler::OnMessage: Сканирование закончено" );
			m_bFinished = true;
			break;
	};
}

enumAgentResponse CAgentHandler::BeginScan( std::vector< std::string > vecAddresses )throw( HandlerErr, CSocket::SocketErr )
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
	SmartPtr< BYTE, AllocMalloc<BYTE> >  pbRecvBuf;
	pbRecvBuf = SendMessage( Msg );
	return (enumAgentResponse)*pbRecvBuf; 
}
	
enumAgentResponse CAgentHandler::StopScan()throw( HandlerErr, CSocket::SocketErr )
{
	Log::instance().Trace( 90, "CAgentHandler::StopScan: Отправка команды окончания сканирования" );
	CPacket Msg;

	Msg.BeginCommand( STOP_SCAN );
	Msg.EndCommand();

	SmartPtr< BYTE, AllocMalloc<BYTE> >  pbRecvBuf;
	pbRecvBuf = SendMessage( Msg );
	return (enumAgentResponse)*pbRecvBuf;		
}
	
enumAgentResponse CAgentHandler::GetStatus( enumAgentState& Status )throw( HandlerErr, CSocket::SocketErr )
{
	Log::instance().Trace( 90, "CAgentHandler::GetStatus: Отправка команды получения статуса" );
	CPacket Msg;

	Msg.BeginCommand( GET_STATUS );
	Msg.EndCommand();

	SmartPtr< BYTE, AllocMalloc<BYTE> >  pbRecvBuf;
	pbRecvBuf = SendMessage( Msg );
	if( RESP_OK != *pbRecvBuf )
	{
		Log::instance().Trace( 50, "CAgentHandler::GetStatus: Команда получения статуса не выполнена, код возврата: %d", *pbRecvBuf );
		return (enumAgentResponse)*pbRecvBuf;
	}
	Status = (enumAgentState)pbRecvBuf.get()[1];
	Log::instance().Trace( 80, "CAgentHandler::GetStatus: Получен статус: %d", Status );
	return RESP_OK;
}
	
enumAgentResponse CAgentHandler::GetData()throw( HandlerErr, CSocket::SocketErr )
{
	Log::instance().Trace( 90, "CAgentHandler::GetData: Отправка команды получения данных" );	
	CPacket Msg;

	Msg.BeginCommand( GET_DATA );
	Msg.EndCommand();

	//отправляем команду
	SmartPtr< BYTE, AllocMalloc<BYTE> >  pbRecvBuf;
	pbRecvBuf = SendMessage( Msg );
	if( RESP_OK != *pbRecvBuf )
	{
		Log::instance().Trace( 50, "CAgentHandler::GetData: Команда получения данных не выполнена, код возврата: %d", *pbRecvBuf );
		return (enumAgentResponse)*pbRecvBuf;
	}
	int iDataSize;
	::memcpy( (BYTE*)&iDataSize, pbRecvBuf.get()+1, 4 );
	Log::instance().Trace( 80, "CAgentHandler::GetData: Размер данных: %d", iDataSize );
	Log::instance().Dump( 90, pbRecvBuf.get()+5, iDataSize, "CAgentHandler::GetData: Получены данные:" );
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
	CPacket Msg;
	BYTE pBuf[10240];
	int iCount;
	while( iCount = pThis->m_pSocket->Receive( pBuf, sizeof( pBuf ) ) )
	{
		Log::instance().Dump( 90, pBuf, iCount, "CConnectionHandler::fnListenThread: Получен пакет:" );
		Msg.SetBuffer( pBuf, iCount );
		pThis->m_pAgentHandler->OnMessage( Msg );
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
