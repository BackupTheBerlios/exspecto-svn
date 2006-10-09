//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: �����, ��������������� �������������� � �������
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
	//������� ������ � ��� m_Sock.GetLastError
	 return true;
}

bool CAgentHandler::Close()
{
	return  0 == m_Sock.Close()?true:false; 
}
/*
//��������� ������� Command �� ����� strAddress � ����������� vcParams
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

//��������� ������� Command �� ����� strAddress � �������� ����� � pBuffer
//iBufSize - ������ ������
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
	//���� ��������� ����� �� ������� - �������� ���
	if( 0 != iBufSize )
        sock.Receive( pBuffer, iSize );
	sock.Close();
	return true;
}*/


