 //------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#include "CAgent.h"
#include "ServerSocket.h"


//�����������,strSchedulerAddress - ����� ������������
CAgent::CAgent( std::string strSchedulerAddress ):m_CurState( Idling )
												 ,m_strSchedulerAddress( strSchedulerAddress )
{
	DWORD dwThreadId;
	//��������� ����� �������������(�������� �������� TCP ����������)
	::CloseHandle( ::CreateThread( 0, 0, fnListenThreadProc, this, 0, &dwThreadId ) );
	::InitializeCriticalSection( &m_csCurState );
	::InitializeCriticalSection( &m_csCommandExec );
	//�������������� ����� ������������ ������
	m_mapHandlers[ GET_STATUS ] = &CAgent::GetStatus;
	m_mapHandlers[ START_SCAN ] = &CAgent::StartScan;
	m_mapHandlers[ GET_DATA ] = &CAgent::GetData;
	m_mapHandlers[ STOP_SCAN ] = &CAgent::StopScan;
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
	CPacket Msg;

	//������ ������ ��� ������� ��������� ������
	Msg.SetBuffer( pParams->pbBuf, pParams->iCount );
	for(;;)
	{
		try{
			 Msg.GetCommandId( bCommandId );
		}catch( CPacket::PacketFormatErr )
		{
			Log::instance().Trace( 11, "CAgent::fnProcessThreadProc: ������ ����� ��������� �������" );
			break;
		}
		(pParams->pThis->*(pParams->pThis->m_mapHandlers[ bCommandId ]))( Msg, pParams->client_sock );
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
		Log::instance().Trace( 51, "CAgent::fnListenThreadProc: �������� ���������� � ������: %s", adr.strAddr.c_str() );
		try{
			//��������� ���������� ������ �� ��������� ������� ������������
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

//����������� ������
enumAgentResponse CAgent::GetStatus( CPacket& Msg, CSocket* pSchedSocket )
{
	Log::instance().Trace( 90, "CAgent: �������� ������ �� ��������� �������" );
	BYTE pbBuf[2];
	pbBuf[0] = 0xFF;
	pbBuf[1] = m_CurState;
	//���������� ������� ���������
	pSchedSocket->Send( pbBuf, 2 );
	//��������� ����������
	pSchedSocket->Close();
	
	return RESP_OK;
}
	
enumAgentResponse CAgent::StartScan( CPacket& Msg, CSocket* pSchedSocket )
{
	Log::instance().Trace( 90, "CAgent: �������� ������ �� ������ ������������" );
	DWORD dwCount = 0;
	std::string strAddress;
	std::vector< std::string > List;
	
	::EnterCriticalSection( &m_csCommandExec );
	//�������� ���-�� ������� � ������
	Msg.GetParam( dwCount );
	Log::instance().Trace( 80, "CAgent:StartScan: ���-�� ������� ��� ������������: %d", dwCount );
	
	::EnterCriticalSection( &m_csCurState );
	m_CurState = Scanning;	
	::LeaveCriticalSection( &m_csCurState );

	for( unsigned int i = 0; i < dwCount; i++ )
	{
		//�������� ��������� ����� � ���������� ��� ������������ �� ���� ��������� ����������
		Msg.GetAddress( strAddress );
		for( PluginIterator It = m_PluginContainer.begin(); It != m_PluginContainer.end(); It++ )
		{
			Log::instance().Trace( 80, "CAgent::StartScan: ��������� ����� %s � ������� ������� %s", strAddress.c_str(), (*It)->GetProtocolName() );
			(*It)->Scan( strAddress, List );
		}
	}
	::EnterCriticalSection( &m_csCurState );
	m_CurState = Idling;		
	::LeaveCriticalSection( &m_csCurState );
	
	std::vector< std::string >::iterator It;
	for( It = List.begin(); It != List.end(); It++ )
	{
		//std::cout << *It << std::endl;
	}
	::LeaveCriticalSection( &m_csCommandExec );
	return RESP_OK;
}

enumAgentResponse CAgent::GetData( CPacket& Msg, CSocket* pSchedSocket )
{
	Log::instance().Trace( 90, "CAgent: �������� ������ �� ��������� ������" );
	return RESP_OK;
}
	
enumAgentResponse CAgent::StopScan( CPacket& Msg, CSocket* pSchedSocket )
{
	Log::instance().Trace( 90, "CAgent: �������� ������ �� ��������� ������������" );
	return RESP_OK;
}

