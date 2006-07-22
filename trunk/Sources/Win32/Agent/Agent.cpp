#pragma once
#include "stdafx.h"
#include "..\net.lib\Sockets.h"
#include ".\agent.h"
#include "windows.h"
#include "..\net.lib\Scanner.h"
#include "..\common\commands.h"
#include "..\net.lib\packet.h"

CAgent::CAgent( std::string strSchedulerAddress ):m_CurState( enumStates::Idling )
												 ,m_strSchedulerAddress( strSchedulerAddress )
{
	DWORD dwThreadId;
	::CloseHandle( ::CreateThread( 0, 0, fnListenThreadProc, this, 0, &dwThreadId ) );
	::InitializeCriticalSection( &m_csCurState );
}

CAgent::~CAgent(void)
{
	::DeleteCriticalSection( &m_csCurState );
}

void CAgent::Process( BYTE* pBuf, int iSize, CSocket* sock )
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
			BYTE pbBuf[2];
			pbBuf[0] = 0xFF;
			pbBuf[1] = m_CurState;
			sock->Send( pbBuf, 2 );
			break;
		case StartScan:
			pPacket->GetParam( iCount );
			pPacket->GetAddress( strAddress );
			CScanner scan;

			::EnterCriticalSection( &m_csCurState );
			m_CurState = enumStates::Scanning;	
			::LeaveCriticalSection( &m_csCurState );

			scan.Scan( strAddress, List );
			
			::EnterCriticalSection( &m_csCurState );
			m_CurState = enumStates::Idling;		
			::LeaveCriticalSection( &m_csCurState );

			std::vector< std::string >::iterator It;
			for( It = List.begin(); It != List.end(); It++ )
			{
				std::cout << *It << std::endl;
			}
			break;
		}
	}
	delete sock;
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
		if( ( pThis->m_strSchedulerAddress == adr.strAddr ) && ( SOCKET_ERROR != ( iCount = client_sock->Receive( pBuf, 10240 ) ) ) )
            pThis->Process( pBuf, iCount, client_sock );
	}
	return DWORD();
}

int _tmain(int argc, _TCHAR* argv[])
{
	CAgent ag( "127.0.0.1" );
	Sleep( 1000000 );
	return 0;
}