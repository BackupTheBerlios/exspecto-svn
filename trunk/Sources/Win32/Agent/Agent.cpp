#pragma once
#include "stdafx.h"
#include ".\agent.h"
#include "..\net.lib\Sockets.h"
#include "..\net.lib\ServerSocket.h"
#include "..\net.lib\ClientSocket.h"
#include "..\common\commands.h"
#include "..\net.lib\packet.h"


int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}


CAgent::CAgent(void)
{
	DWORD dwThreadId;
	::CloseHandle( ::CreateThread( 0, 0, fnListenThreadProc, this, 0, &dwThreadId ) );
}

CAgent::~CAgent(void)
{
}

void CAgent::StateMachine()
{
}

void CAgent::Parse( BYTE* pBuf, int iSize )
{
	BYTE bCommandId;
	CPacket* pPacket = new CPacket();

	pPacket->SetBuffer( pBuf, iSize );
	while( pPacket->GetCommandId( bCommandId ) )
	{
		switch( bCommandId )
		{
		case GetStatus:
			break;
		case StartScan:
			break;
		}
	}
}

DWORD WINAPI CAgent::fnListenThreadProc(  void* pParameter )
{
	CAgent* pThis = (CAgent*)pParameter;
	CServerSocket sock;
	CSocket* client_sock;
	BYTE pBuf[10240];
	int iCount = 0;

	structAddr adr;

	sock.Bind();
	sock.Listen();
	while( NULL != ( client_sock = sock.Accept( adr ) ) )
	{
		if( SOCKET_ERROR != ( iCount = sock.Receive( pBuf, 10240 ) ) )
            pThis->Parse( pBuf, iCount );
	}
	return DWORD();
}