//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
//-------------------------------------------------------------------------------------//
#include "CAgentHandler.h"

CAgentHandler::CAgentHandler( std::string strAgentAddress ):m_strAddress( strAgentAddress )
{
}

CAgentHandler::~CAgentHandler()
{
}

enumCommandResult CAgentHandler::SendMessage( CPacket &Msg )
{
	int iSize = 0;
	BYTE* pBuf;
	Msg.GetBuffer( pBuf, iSize );
	if( 0 != iSize )
		m_Sock.Send( pBuf, iSize );
	else;
		//TODO:
	return RES_OK;
}

bool CAgentHandler::Open()
{
	if( 0 != m_Sock.Connect( m_strAddress, PORT ) )
	{
		//TODO:
	};
	//вывести ошибку в лог m_Sock.GetLastError
	 return true;
}

bool CAgentHandler::Close()
{
	return  0 == m_Sock.Close()?true:false; 
}
/*
//Отправить команду Command на адрес strAddress с параметрами vcParams
bool CScanServer::SendCommand( )
{
	CClientSocket sock;
	CPacket Msg;
	BYTE* pBuf = NULL;
	int iSize;

	switch( Command )
	{
	case StartScan:
		{
			Msg.BeginCommand( Command );
			Msg.AddParam( (DWORD)vcParams.size() );
			for( std::vector< std::string >::iterator It = vcParams.begin(); It != vcParams.end(); It++ )
				Msg.AddAddress( *It );
			Msg.EndCommand();
			Msg.GetBuffer( pBuf, iSize );
		}break;
	case GetStatus:
		{
		}break;
	}

	sock.Connect( strAddress, 5000 );
	sock.Send( pBuf, iSize );
	sock.Close();
	return true;
}

//Отправить команду Command на адрес strAddress и получить ответ в pBuffer
//iBufSize - размер буфера
bool CScanServer::SendCommand( std::string strAddress, enumCommands Command, BYTE* pBuffer, int iBufSize )
{
	CClientSocket sock;
	CPacket Msg;
	BYTE* pBuf = NULL;
	int iSize;

	Msg.BeginCommand( Command );
	Msg.EndCommand();
	Msg.GetBuffer( pBuf, iSize );

	sock.Connect( strAddress, 5000 );
	
	sock.Send( pBuf, iSize );
	//если ожидается ответ на команду - получить его
	if( 0 != iBufSize )
        sock.Receive( pBuffer, iSize );
	sock.Close();
	return true;
}*/


