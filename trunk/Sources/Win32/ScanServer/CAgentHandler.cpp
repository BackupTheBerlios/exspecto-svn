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
	if( 0 == iSize )
		throw HandlerErr( "Attempt to send message with zero-length was made" );		
	m_Sock.Send( pBuf, iSize );


	//Получаем ответ на сообщение
	int iRecvRes = 0;
	iRecvRes = m_Sock.Receive( pbRespBuf, iRespSize ); 
	if( 0 == iRecvRes )
		throw HandlerErr( "Zero length response received" );
	if( iRecvRes != iRespSize )
		throw HandlerErr( "Received response with incorrect size" );						//Ожидаемый размер не совпал с фактическим
}

void CAgentHandler::Open()throw( CSocket::SocketErr )
{
	m_Sock.Connect( m_strAddress, PORT );
	m_bOpened = true;
}

void CAgentHandler::Close()throw( CSocket::SocketErr )
{
	m_Sock.Close();
	m_bOpened = false;
}

bool CAgentHandler::IsOpened()const
{
	return m_bOpened;
}

enumAgentResponse CAgentHandler::BeginScan( std::vector< std::string > vecAddresses )throw( HandlerErr, CSocket::SocketErr )
{
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( START_SCAN );
	Msg.AddParam( (DWORD)vecAddresses.size() );
	for( std::vector< std::string >::iterator It = vecAddresses.begin(); It != vecAddresses.end(); It++ )
		Msg.AddAddress( *It );
	Msg.EndCommand();
	
	
	SendMessage( Msg, pbRecvBuf, 1 );
	return (enumAgentResponse)pbRecvBuf[0]; 
}
	
enumAgentResponse CAgentHandler::StopScan()throw( HandlerErr, CSocket::SocketErr )
{
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( STOP_SCAN );
	Msg.EndCommand();

	SendMessage( Msg, pbRecvBuf, 1 );
	return (enumAgentResponse)pbRecvBuf[0];		
}
	
enumAgentResponse CAgentHandler::GetStatus( enumAgentStatus& Status )throw( HandlerErr, CSocket::SocketErr )
{
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( GET_STATUS );
	Msg.EndCommand();

	SendMessage( Msg, pbRecvBuf, 1 );
	if( RESP_OK != pbRecvBuf[0] )
		return (enumAgentResponse)pbRecvBuf[0];
	SendMessage( Msg, pbRecvBuf, 1 );
	Status = (enumAgentStatus)pbRecvBuf[0];
	return RESP_OK;
}
	
enumAgentResponse CAgentHandler::GetData()throw( HandlerErr, CSocket::SocketErr )
{
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( GET_DATA );
	Msg.EndCommand();

	//отправляем команду
	SendMessage( Msg, pbRecvBuf, 1 );
	if( RESP_OK != pbRecvBuf[0] )
		return (enumAgentResponse)pbRecvBuf[0];
	//ещё раз,в ответ должен прийти размер данных
	SendMessage( Msg, pbRecvBuf, 3 );
	int iDataSize;
	::memcpy( (BYTE*)&iDataSize + 3, pbRecvBuf, 3 );
	BYTE* pbData = new BYTE[ iDataSize ];
	//последний раз, в ответ приходят данные
	SendMessage( Msg, pbData, iDataSize );
	delete pbData;
	return RESP_OK;
}


