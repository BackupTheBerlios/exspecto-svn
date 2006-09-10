//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции планировщика
//-------------------------------------------------------------------------------------//
#include "stdafx.h"
#include "winsock2.h"
#include "..\net.lib\ClientSocket.h"
#include "..\net.lib\Packet.h"
#include ".\scheduler.h"
#include "conio.h"

CScheduler::CScheduler(void)
{
}

CScheduler::~CScheduler(void)
{
}

/*void CScheduler::FindAgents( std::vector< std::string > &m_AgentList )
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

//Отправить команду Command на адрес strAddress с параметрами vcParams
bool CScheduler::SendCommand( std::string strAddress, enumCommands Command, std::vector< std::string > vcParams )
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
bool CScheduler::SendCommand( std::string strAddress, enumCommands Command, BYTE* pBuffer, int iBufSize )
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
}

int _tmain(int argc, _TCHAR* argv[])
{
	CScheduler shed;
	BYTE pBuf[1024];

	std::vector< std::string > vcAddresses;

	vcAddresses.push_back( "172.16.3.39" );
	vcAddresses.push_back( "172.16.3.110" );
	vcAddresses.push_back( "172.16.3.121" );

	shed.SendCommand( "127.0.0.1", StartScan, vcAddresses );
	shed.SendCommand( "127.0.0.1", GetStatus, pBuf, sizeof( pBuf ) );
	printf( "%02X ",pBuf[0] );
	printf( "%02X ",pBuf[1] );
	char c = getch();
	return 0;
}
