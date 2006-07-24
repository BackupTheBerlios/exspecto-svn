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

DWORD WINAPI CAgent::fnProcessThreadProc( LPVOID pParameter )
{
	ProcessParam* pParams = (ProcessParam*)pParameter;
	BYTE bCommandId;
	DWORD iCount;
	std::string strAddress;
	std::vector< std::string > List;

	CPacket* pPacket = new CPacket();

	pPacket->SetBuffer( pParams->pbBuf, pParams->iCount );
	while( pPacket->GetCommandId( bCommandId ) )
	{
		switch( bCommandId )
		{
		case GetStatus:
			BYTE pbBuf[2];
			pbBuf[0] = 0xFF;
			pbBuf[1] = pParams->pThis->m_CurState;
			pParams->client_sock->Send( pbBuf, 2 );
			break;
		case StartScan:
			//TODO:добавить синхронизацию
			pPacket->GetParam( iCount );
			CScanner scan;

			::EnterCriticalSection( &pParams->pThis->m_csCurState );
			pParams->pThis->m_CurState = enumStates::Scanning;	
			::LeaveCriticalSection( &pParams->pThis->m_csCurState );

			for( unsigned int i = 0; i < iCount; i++ )
			{
				pPacket->GetAddress( strAddress );
				scan.Scan( strAddress, List );
			}
			::EnterCriticalSection( &pParams->pThis->m_csCurState );
			pParams->pThis->m_CurState = enumStates::Idling;		
			::LeaveCriticalSection( &pParams->pThis->m_csCurState );

			std::vector< std::string >::iterator It;
			for( It = List.begin(); It != List.end(); It++ )
			{
				std::cout << *It << std::endl;
			}
			break;
		}
	}
	delete pParams->client_sock;
	delete pPacket;
	delete pParams;
	return 0;
}

void CAgent::Process( BYTE* pBuf, int iSize, CSocket* sock )
{
	return;
}

DWORD WINAPI CAgent::fnListenThreadProc(  void* pParameter )
{
	CAgent* pThis = (CAgent*)pParameter;
	CServerSocket sock;
	CSocket* client_sock;
	BYTE pBuf[10240];
	int iCount = 0;
	DWORD dwThreadId;

	structAddr adr;

	sock.SetBlocking( true );
	sock.Bind( 5000, "127.0.0.1" );
	sock.Listen();
	while( NULL != ( client_sock = sock.Accept( adr ) ) )
	{
		if( ( pThis->m_strSchedulerAddress == adr.strAddr ) && ( SOCKET_ERROR != ( iCount = client_sock->Receive( pBuf, 10240 ) ) ) )
		{
			ProcessParam* params = new ProcessParam;
			params->client_sock = client_sock;
			params->iCount = iCount;
			params->pbBuf = pBuf;
			params->pThis = pThis;
            //pThis->Process( pBuf, iCount, client_sock );
			::CloseHandle( ::CreateThread( 0, 0, fnProcessThreadProc, params, 0, &dwThreadId ) );
		}
	}
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	CAgent ag( "127.0.0.1" );
	Sleep( 1000000 );
	return 0;
}