#include "stdafx.h"
#include ".\sheduler.h"
#include "winsock2.h"
#include "..\net.lib\ClientSocket.h"
#include "..\net.lib\Packet.h"

CSheduler::CSheduler(void)
{
}

CSheduler::~CSheduler(void)
{
}

void CSheduler::FindAgents( std::vector< std::string > &m_AgentList )
{
}

/*void CSheduler::AddAgent( std::string strAddress )
{
	m_iCurId++;
	m_AgentList[ m_iCurId ] = strAddress;
}

void CSheduler::RemoveAgent( int iAgentId )
{
}

/*int CSheduler::GetAgentId( std::string strAdress )
{
	return int();
}*/

/*std::string CSheduler::GetAgentAddress( int iAgentId )
{
	return std::string();
}*/

bool CSheduler::SendCommand( std::string strAddress, enumCommands Command )
{
	CClientSocket sock( AF_INET );
	CPacket Msg;
	BYTE* pBuf = NULL;
	int iSize;

	Msg.BeginCommand( Command );
		Msg.AddParam( 1 );
		Msg.AddParam( "172.16.4.59" );
	Msg.EndCommand();
	Msg.GetBuffer( pBuf, iSize );

	sock.Connect( strAddress, 5000 );
	
	sock.Send( pBuf, iSize );
	sock.Close();
	return bool();
}

bool CSheduler::SendCommand( int iAgentId, enumCommands Command )
{
	return bool();
}

int _tmain(int argc, _TCHAR* argv[])
{
	CSheduler shed;

	shed.SendCommand( "127.0.0.1", StartScan );
	return 0;
}