 //------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#include <iostream>
#include <tchar.h>
#include <vector>
#include "..\libNet\Sockets.h"
#include "agent.h"
#include "windows.h"
#include "..\libNet\packet.h"
#include "constants.h"

//�����������,strSchedulerAddress - ����� ������������
CAgent::CAgent( std::string strSchedulerAddress ):m_CurState( Idling )
												 ,m_strSchedulerAddress( strSchedulerAddress )
{
	DWORD dwThreadId;
	FillScannersList();
	//��������� ����� �������������(�������� �������� TCP ����������)
	::CloseHandle( ::CreateThread( 0, 0, fnListenThreadProc, this, 0, &dwThreadId ) );
	::InitializeCriticalSection( &m_csCurState );
	::InitializeCriticalSection( &m_csCommandExec );
}

CAgent::~CAgent(void)
{
	ClearScannersList();
	::DeleteCriticalSection( &m_csCurState );
	::DeleteCriticalSection( &m_csCommandExec );
}

//��������� vecScanners
int CAgent::FillScannersList()
{
	WIN32_FIND_DATA FindData;
	HANDLE hFindFile;
	HINSTANCE hLib;
	CScanner* pScanner;
	fnGetScannerFunc fnGetScanner;
	fnReleaseScannerFunc fnReleaseScanner;
	int iScannersCount = 0;
	std::string strPluginPath;// = PLUGIN_PATH;
	strPluginPath += "*.dll";
	//������� ��� dll � ����� � plugin-���
	if( INVALID_HANDLE_VALUE == ( hFindFile = ::FindFirstFile( strPluginPath.c_str(), &FindData ) ) )
	{
		return 0;
	}
	do
	{ 
		if( NULL == ( hLib = ::LoadLibraryA( FindData.cFileName ) )
			|| NULL == ( fnGetScanner = ( fnGetScannerFunc )::GetProcAddress( hLib, "GetScanner" ) )
		 	|| NULL == ( fnReleaseScanner = ( fnReleaseScannerFunc )::GetProcAddress( hLib, "ReleaseScanner" )) 
		 	|| NULL == ( pScanner = fnGetScanner() ) )
		{
			::FreeLibrary( hLib );
			continue;
		}
		//��������� ������ m_mapLibraries ��� ���������� ���������� �������� dll � ��������
		//���������������� ������� CScanner
		m_mapLibraries[ hLib ] = fnReleaseScanner;

		m_vecScanners.push_back( pScanner );
		iScannersCount++;
	}while( ::FindNextFile( hFindFile, &FindData ) );
	return iScannersCount;
}	

//������� m_vecScanners � m_mapLibraries
void CAgent::ClearScannersList()
{
	for( LibrariesMapType::iterator It = m_mapLibraries.begin(); It != m_mapLibraries.end(); It++ )
	{
		It->second();
		::FreeLibrary( It->first );
	}
}

//����� ��������� �������� ���������
DWORD WINAPI CAgent::fnProcessThreadProc( LPVOID pParameter )
{
	ProcessParam* pParams = (ProcessParam*)pParameter;
	BYTE bCommandId;
	DWORD iCount;
	std::string strAddress;
	std::vector< std::string > List;

	CPacket* pPacket = new CPacket();

	//������ ������ ��� ������� ��������� ������
	pPacket->SetBuffer( pParams->pbBuf, pParams->iCount );
	while( pPacket->GetCommandId( bCommandId ) )
	{
		switch( bCommandId )
		{
		case GetStatus:
			BYTE pbBuf[2];
			pbBuf[0] = 0xFF;
			pbBuf[1] = pParams->pThis->m_CurState;
			//���������� ������� ���������
			pParams->client_sock->Send( pbBuf, 2 );
			//��������� ����������
			pParams->client_sock->Close();
			break;
		case StartScan:
			::EnterCriticalSection( &pParams->pThis->m_csCommandExec );
			//�������� ���-�� ������� � ������
			pPacket->GetParam( iCount );



			::EnterCriticalSection( &pParams->pThis->m_csCurState );
			pParams->pThis->m_CurState = Scanning;	
			::LeaveCriticalSection( &pParams->pThis->m_csCurState );

			for( unsigned int i = 0; i < iCount; i++ )
			{
				//�������� ��������� ����� � ���������� ��� ������������ �� ���� ��������� ����������
				pPacket->GetAddress( strAddress );
				for( std::vector< CScanner* >::iterator It = pParams->pThis->m_vecScanners.begin(); It != pParams->pThis->m_vecScanners.end(); It++ )
				{
					(*It)->Scan( strAddress, List );
				}
			}
			::EnterCriticalSection( &pParams->pThis->m_csCurState );
			pParams->pThis->m_CurState = Idling;		
			::LeaveCriticalSection( &pParams->pThis->m_csCurState );

			std::vector< std::string >::iterator It;
			for( It = List.begin(); It != List.end(); It++ )
			{
				std::cout << *It << std::endl;
			}
			::LeaveCriticalSection( &pParams->pThis->m_csCommandExec );
			break;
		}
	}
	delete pParams->client_sock;
	delete pPacket;
	delete pParams;
	return 0;
}

//����� �������� �������� ����������
DWORD WINAPI CAgent::fnListenThreadProc(  void* pParameter )
{
	CAgent* pThis = (CAgent*)pParameter;
	CServerSocket sock;
	CSocket* client_sock;
	BYTE pBuf[10240];
	int iCount = 0;
	DWORD dwThreadId;

	structAddr adr;

    //��������� ��������� ����� � ��������� �������
	sock.Bind( 5000, "127.0.0.1" );
	//��������� ����� � ����� �������������
	sock.Listen();
	//������� �������� ���������� � ������������ ���
	while( NULL != ( client_sock = sock.Accept( adr ) ) )
	{
		//��������� ���������� ������ �� ��������� ������������
		if( ( pThis->m_strSchedulerAddress == adr.strAddr ) && ( SOCKET_ERROR != ( iCount = client_sock->Receive( pBuf, 10240 ) ) ) )
		{
			ProcessParam* params = new ProcessParam;
			params->client_sock = client_sock;
			params->iCount = iCount;
			params->pbBuf = pBuf;
			params->pThis = pThis;
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
