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

CAgentHandler::~CAgentHandler()
{
	//Перед уничтожением закрываем соединение с агентом
	Close();
}

//Отправить пакет Msg агенту и получить ответ в pbRespBuf, iRespSize - ожидаемый размер ответа
CAgentHandler::enumCommandResult CAgentHandler::SendMessage( CPacket &Msg, BYTE* pbRespBuf, int iRespSize )
{
	if( !m_bOpened )
		return RES_NOT_OPENED;

	BYTE* pBuf = NULL;
	int iSize = 0;
	//Получаем буфер с данными сообщения Msg для отправки по сети
	Msg.GetBuffer( pBuf, iSize );
	if( 0 != iSize )
		m_Sock.Send( pBuf, iSize );
	else;
		//TODO:

	if( SOCKET_ERROR == m_Sock.Send( pBuf, iSize ) )
	{
		//TODO:
		return RES_SOCK_ERR;
	}
	//Получаем ответ на сообщение
	int iRecvRes = 0;
	if( SOCKET_ERROR == ( iRecvRes = m_Sock.Receive( pbRespBuf, iRespSize ) || 0 == iRecvRes ) )
	{
		if( WSAEMSGSIZE == m_Sock.GetLastError() )
			return RES_INCORRECT;					//Ожидаемый размер ответа оказался меньше фактического
		//TODO:
		return RES_NO_RESPONSE;
	}
	if( iRecvRes != iRespSize )
	{
		//TODO:
		return RES_INCORRECT;						//Ожидаемый размер не совпал с фактическим
	}
	return RES_OK;		
}

bool CAgentHandler::Open()
{
	if( 0 != m_Sock.Connect( m_strAddress, PORT ) )
	{
		//TODO:
		return false;
	};
	m_bOpened = false;
	//вывести ошибку в лог m_Sock.GetLastError
	 return true;
}

bool CAgentHandler::Close()
{
	if( 0 != m_Sock.Close() )
	{
		//TODO:
		return false;
	}
	m_bOpened = false;
	return  true; 
}

bool CAgentHandler::IsOpened()
{
	return m_bOpened;
}

CAgentHandler::enumCommandResult CAgentHandler::BeginScan( std::vector< std::string > vecAddresses )
{
	CPacket Msg;
	BYTE pbRecvBuf[255];
	enumCommandResult Res;

	Msg.BeginCommand( START_SCAN );
	Msg.AddParam( (DWORD)vecAddresses.size() );
	for( std::vector< std::string >::iterator It = vecAddresses.begin(); It != vecAddresses.end(); It++ )
		Msg.AddAddress( *It );
	Msg.EndCommand();
	
	
	if( RES_OK != ( Res = SendMessage( Msg, pbRecvBuf, 1 ) ) )
	{
		//TODO:
		return Res;
	} 
	return RESP_OK == pbRecvBuf[0]? RES_OK:RES_AGENT_ERR; 
}
	
CAgentHandler::enumCommandResult CAgentHandler::StopScan()
{
	CPacket Msg;
	BYTE pbRecvBuf[255];
	enumCommandResult Res;

	Msg.BeginCommand( STOP_SCAN );
	Msg.EndCommand();

	if( RES_OK != ( Res = SendMessage( Msg, pbRecvBuf, 1 ) ) )
	{
		//TODO:
		return Res;
	} 

	return RESP_OK == pbRecvBuf[0]? RES_OK:RES_AGENT_ERR;;		
}
	
CAgentHandler::enumCommandResult CAgentHandler::GetStatus( enumAgentStatus& Status )
{
	CPacket Msg;
	BYTE pbRecvBuf[255];
	enumCommandResult Res;

	Msg.BeginCommand( GET_STATUS );
	Msg.EndCommand();

	if( RES_OK != ( Res = SendMessage( Msg, pbRecvBuf, 1 ) ) )
	{
		//TODO:
		return Res;
	} 
	if( RESP_OK != pbRecvBuf[0] )
	{
		//TODO:
		return RES_AGENT_ERR;
	}
	if( RES_OK != ( Res = SendMessage( Msg, pbRecvBuf, 1 ) ) )
	{
		//TODO:
		return Res;
	} 
	Status = (enumAgentStatus)pbRecvBuf[0];
	return RES_OK;
}
	
CAgentHandler::enumCommandResult CAgentHandler::GetData()
{
	CPacket Msg;
	BYTE pbRecvBuf[255];
	enumCommandResult Res;

	Msg.BeginCommand( GET_DATA );
	Msg.EndCommand();

	if( RES_OK != ( Res = SendMessage( Msg, pbRecvBuf, 1 ) ) )
	{
		//TODO:
		return Res;
	} 
	if( RESP_OK != pbRecvBuf[0] )
	{
		//TODO:
		return RES_AGENT_ERR;
	}
	if( RES_OK != ( Res = SendMessage( Msg, pbRecvBuf, 3 ) ) )
	{
		//TODO:
		return Res;
	} 
	int iDataSize;
	::memcpy( (BYTE*)&iDataSize + 3, pbRecvBuf, 3 );
	BYTE* pbData = new BYTE[ iDataSize ];
	if( RES_OK != ( Res = SendMessage( Msg, pbData, iDataSize ) ) )
	{
		//TODO:
		return Res;
	} 
	delete pbData;
	return RES_OK;
}


