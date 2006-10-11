//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������
//-------------------------------------------------------------------------------------//
#include <iostream>
#include <tchar.h>
#include "winsock2.h"
#include "..\libNet\ClientSocket.h"
#include "..\libNet\Packet.h"
#include ".\ScanServer.h"
#include "conio.h"

CScanServer::CScanServer(void)
{
}

CScanServer::~CScanServer(void)
{
}

/*void CScanServer::FindAgents( std::vector< std::string > &m_AgentList )
{
}
*/
/*void CSheduler::AddAgent( std::string strAddress )
{
	m_iCurId++;
	m_AgentList[ m_iCurId ] = strAddress;
}

void CSheduler::RemoveAgent( int iAgentId )
{
}

int CSheduler::GetAgentId( std::string strAdress )
{
	return int();
}*/

/*std::string CSheduler::GetAgentAddress( int iAgentId )
{
	return std::string();
}*/

//��������� ������� Command �� ����� strAddress � ����������� vcParams
bool CScanServer::SendCommand( std::string strAddress, enumCommands Command, std::vector< std::string > vcParams )
{
	CClientSocket sock;
	CPacket Msg;
	BYTE* pBuf = NULL;
	int iSize;

/*	switch( Command )
	{
	case START_SCAN:
		{
			Msg.BeginCommand( Command );
			Msg.AddParam( (DWORD)vcParams.size() );
			for( std::vector< std::string >::iterator It = vcParams.begin(); It != vcParams.end(); It++ )
				Msg.AddAddress( *It );
			Msg.EndCommand();
			Msg.GetBuffer( pBuf, iSize );
		}break;
	case GET_STATUS:
		{
		}break;
	}*/

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
}

int _tmain(int argc, _TCHAR* argv[])
{
	CScanServer shed;
	BYTE pBuf[1024];

	std::vector< std::string > vcAddresses;

	vcAddresses.push_back( "172.16.3.39" );
	vcAddresses.push_back( "172.16.3.110" );
	vcAddresses.push_back( "172.16.3.121" );

	shed.SendCommand( "127.0.0.1", START_SCAN, vcAddresses );
	shed.SendCommand( "127.0.0.1", GET_STATUS, pBuf, sizeof( pBuf ) );
	printf( "%02X ",pBuf[0] );
	printf( "%02X ",pBuf[1] );
	getch();
	return 0;
}
