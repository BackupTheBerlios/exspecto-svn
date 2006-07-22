#pragma once
#include "stdafx.h"
#include "..\net.lib\Sockets.h"
#include ".\agent.h"
#include "windows.h"
#include "..\net.lib\Scanner.h"
#include "..\common\commands.h"
#include "..\net.lib\packet.h"

CAgent::CAgent(void):m_CurState( enumStates::Idling )
{
	DWORD dwThreadId;
	::CloseHandle( ::CreateThread( 0, 0, fnListenThreadProc, this, 0, &dwThreadId ) );
}

CAgent::~CAgent(void)
{
}

void CAgent::Process( BYTE* pBuf, int iSize )
{

	BYTE bCommandId;
	DWORD iCount;
	std::string strAddress;
	std::vector< std::string > List;

	CPacket* pPacket = new CPacket();

	pPacket->SetBuffer( pBuf, iSize );
	while( pPacket->GetCommandId( bCommandId ) )
	{
		switch( bCommandId )
		{
		case GetStatus:
			m_CurState = enumStates::SendingData;
			break;
		case StartScan:
			pPacket->GetParam( iCount );
			pPacket->GetAddress( strAddress );
			CScanner scan;

			m_CurState = enumStates::Scanning;			
			scan.Scan( strAddress, List );
			m_CurState = enumStates::Idling;			
			std::vector< std::string >::iterator It;
			for( It = List.begin(); It != List.end(); It++ )
			{
				std::cout << *It << std::endl;
			}
			break;
		}
	}
	delete pPacket;
}

DWORD WINAPI CAgent::fnListenThreadProc(  void* pParameter )
{
	CAgent* pThis = (CAgent*)pParameter;
	CServerSocket sock;
	CSocket* client_sock;
	BYTE pBuf[10240];
	int iCount = 0;

	structAddr adr;

	sock.SetBlocking( true );
	sock.Bind( 5000, "127.0.0.1" );
	sock.Listen();
	while( NULL != ( client_sock = sock.Accept( adr ) ) )
	{
		if( SOCKET_ERROR != ( iCount = client_sock->Receive( pBuf, 10240 ) ) )
            pThis->Process( pBuf, iCount );
	}
	return DWORD();
}

int _tmain(int argc, _TCHAR* argv[])
{
	CAgent ag;
	Sleep( 1000000 );
	return 0;
}