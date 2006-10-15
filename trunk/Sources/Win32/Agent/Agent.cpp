 //------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#include "agent.h"
#include "..\libNet\ServerSocket.h"
#include "constants.h"


//�����������,strSchedulerAddress - ����� ������������
CAgent::CAgent( std::string strSchedulerAddress ):m_CurState( Idling )
												 ,m_strSchedulerAddress( strSchedulerAddress )
{
	DWORD dwThreadId;
	//��������� ����� �������������(�������� �������� TCP ����������)
	::CloseHandle( ::CreateThread( 0, 0, fnListenThreadProc, this, 0, &dwThreadId ) );
	::InitializeCriticalSection( &m_csCurState );
	::InitializeCriticalSection( &m_csCommandExec );
}

CAgent::~CAgent(void)
{
	::DeleteCriticalSection( &m_csCurState );
	::DeleteCriticalSection( &m_csCommandExec );
}

//����� ��������� �������� ���������
DWORD WINAPI CAgent::fnProcessThreadProc( LPVOID pParameter )
{
	ProcessParam* pParams = (ProcessParam*)pParameter;
	BYTE bCommandId;
	DWORD iCount;
	std::string strAddress;
	std::vector< std::string > List;

	std::auto_ptr< CPacket > pPacket = std::auto_ptr< CPacket >( new CPacket() );

	//������ ������ ��� ������� ��������� ������
	pPacket->SetBuffer( pParams->pbBuf, pParams->iCount );
	for(;;)
	{
		try{
			 pPacket->GetCommandId( bCommandId );
		}catch( CPacket::PacketFormatErr )
		{
			break;
		}
		switch( bCommandId )
		{
		case GET_STATUS:
			BYTE pbBuf[2];
			pbBuf[0] = 0xFF;
			pbBuf[1] = pParams->pThis->m_CurState;
			//���������� ������� ���������
			pParams->client_sock->Send( pbBuf, 2 );
			//��������� ����������
			pParams->client_sock->Close();
			break;
		case START_SCAN:
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
				for( PluginIterator It = pParams->pThis->m_PluginContainer.begin(); It != pParams->pThis->m_PluginContainer.end(); It++ )
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
				//std::cout << *It << std::endl;
			}
			::LeaveCriticalSection( &pParams->pThis->m_csCommandExec );
			break;
		}
	}
	delete pParams->client_sock;
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

	CServerSocket::structAddr adr;

    //��������� ��������� ����� � ��������� �������
	sock.Bind( 5000, "127.0.0.1" );
	//��������� ����� � ����� �������������
	sock.Listen();
	//������� �������� ���������� � ������������ ���
	while( NULL != ( client_sock = sock.Accept( adr ) ) )
	{
		try{
			//��������� ���������� ������ �� ��������� ������������
			if( ( pThis->m_strSchedulerAddress == adr.strAddr ) && ( ( iCount = client_sock->Receive( pBuf, 10240 ) ) ) )
			{
				ProcessParam* params = new ProcessParam;
				params->client_sock = client_sock;
				params->iCount = iCount;
				params->pbBuf = pBuf;
				params->pThis = pThis;
				::CloseHandle( ::CreateThread( 0, 0, fnProcessThreadProc, params, 0, &dwThreadId ) );
			}
		}catch( CSocket::SocketErr )
		{
			//���� �������� ����� ������ 10��
			continue;
		}
	}
	return 0;
}


int main(int argc, _TCHAR* argv[])
{
	try
	{
		CAgent ag( "127.0.0.1" );
		Sleep( 1000000 );
	}catch(...)
	{
		MessageBox( NULL, "Some error occured", "Error:", MB_OK );
	}
	return 0;
}
