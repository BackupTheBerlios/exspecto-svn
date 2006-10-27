//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
//-------------------------------------------------------------------------------------//
#include "CAgentHandler.h"

CAgentHandler::CAgentHandler( std::string strAgentAddress ):m_strAddress( strAgentAddress )
														   ,m_bOpened( false )
{
}

CAgentHandler::~CAgentHandler()throw( CSocket::SocketErr )
{
	//Перед уничтожением закрываем соединение с агентом
	Close();
}

//Отправить пакет Msg агенту и получить ответ в pbRespBuf, iRespSize - ожидаемый размер ответа
void CAgentHandler::SendMessage( CPacket &Msg, BYTE* pbRespBuf, int iRespSize )throw( HandlerErr, CSocket::SocketErr )
{
	if( !m_bOpened )
		throw HandlerErr( "Session has not been opened, but SendMessage called" );

	BYTE* pBuf = NULL;
	int iSize = 0;
	//Получаем буфер с данными сообщения Msg для отправки по сети
	Msg.GetBuffer( pBuf, iSize );
	log.Dump( 80, pBuf, iSize, "CAgentHandler::SendMessage: отправляем буфер:" );
	if( 0 == iSize )
		throw HandlerErr( "Attempt to send message with zero-length was made" );		
	m_Sock.Send( pBuf, iSize );


	//Получаем ответ на сообщение
	int iRecvRes = 0;
	iRecvRes = m_Sock.Receive( pbRespBuf, iRespSize );
	log.Dump( 80, pbRespBuf, iRespSize, "CAgentHandler::SendMessage: Получили ответ:" );
	if( 0 == iRecvRes )
		throw HandlerErr( "Zero length response received" );
	if( iRecvRes != iRespSize )
		throw HandlerErr( "Received response with incorrect size" );						//Ожидаемый размер не совпал с фактическим
}

void CAgentHandler::Open()throw( CSocket::SocketErr )
{
	log.Trace( 90, "CAgentHandler::Open: открытие" );
	m_Sock.Connect( m_strAddress, PORT );
	m_bOpened = true;
}

void CAgentHandler::Close()throw( CSocket::SocketErr )
{
	log.Trace( 90, "CAgentHandler::Close: закрытие" );
	m_Sock.Close();
	m_bOpened = false;
}

bool CAgentHandler::IsOpened()const
{
	return m_bOpened;
}

enumAgentResponse CAgentHandler::BeginScan( std::vector< std::string > vecAddresses )throw( HandlerErr, CSocket::SocketErr )
{
	log.Trace( 90, "CAgentHandler::BeginScan: Отправка команды начала сканирования" );
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( START_SCAN );
	log.Trace( 90, "CAgentHandler::BeginScan: Всего адресов: %d", vecAddresses.size() );
	Msg.AddParam( (DWORD)vecAddresses.size() );
	for( std::vector< std::string >::iterator It = vecAddresses.begin(); It != vecAddresses.end(); It++ )
	{
		log.Trace( 92, "CAgentHandler::BeginScan: Добавляем адрес %s", It->c_str() );
		Msg.AddAddress( *It );
	}
	Msg.EndCommand();
	
	
	SendMessage( Msg, pbRecvBuf, 1 );
	return (enumAgentResponse)pbRecvBuf[0]; 
}
	
enumAgentResponse CAgentHandler::StopScan()throw( HandlerErr, CSocket::SocketErr )
{
	log.Trace( 90, "CAgentHandler::StopScan: Отправка команды окончания сканирования" );
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( STOP_SCAN );
	Msg.EndCommand();

	SendMessage( Msg, pbRecvBuf, 1 );
	return (enumAgentResponse)pbRecvBuf[0];		
}
	
enumAgentResponse CAgentHandler::GetStatus( enumAgentState& Status )throw( HandlerErr, CSocket::SocketErr )
{
	log.Trace( 90, "CAgentHandler::StopScan: Отправка команды получения статуса" );
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( GET_STATUS );
	Msg.EndCommand();

	SendMessage( Msg, pbRecvBuf, 1 );
	if( RESP_OK != pbRecvBuf[0] )
	{
		log.Trace( 50, "CAgentHandler::GetStatus: Команда получения статуса не выполнена, код возврата: %d", pbRecvBuf[0] );
		return (enumAgentResponse)pbRecvBuf[0];
	}
	SendMessage( Msg, pbRecvBuf, 1 );
	Status = (enumAgentState)pbRecvBuf[0];
	log.Trace( 80, "CAgentHandler::GetStatus: Получен статус: %d", Status );
	return RESP_OK;
}
	
enumAgentResponse CAgentHandler::GetData()throw( HandlerErr, CSocket::SocketErr )
{
	log.Trace( 90, "CAgentHandler::GetData: Отправка команды получения данных" );	
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( GET_DATA );
	Msg.EndCommand();

	//отправляем команду
	SendMessage( Msg, pbRecvBuf, 1 );
	if( RESP_OK != pbRecvBuf[0] )
	{
		log.Trace( 50, "CAgentHandler::GetData: Команда получения статуса не выполнена, код возврата: %d", pbRecvBuf[0] );
		return (enumAgentResponse)pbRecvBuf[0];
	}
	//ещё раз,в ответ должен прийти размер данных
	SendMessage( Msg, pbRecvBuf, 4 );
	int iDataSize;
	::memcpy( (BYTE*)&iDataSize + 4, pbRecvBuf, 4 );
	log.Trace( 80, "CAgentHandler::GetData: Размер данных: %d", iDataSize );
	BYTE* pbData = new BYTE[ iDataSize ];
	//последний раз, в ответ приходят данные
	SendMessage( Msg, pbData, iDataSize );
	log.Dump( 90, pbData, iDataSize, "CAgentHandler::GetData: Получены данные:" );
	delete pbData;
	return RESP_OK;
}


