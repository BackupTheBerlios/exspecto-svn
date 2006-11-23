 //------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#include "CAgent.h"
#include "ServerSocket.h"


//�����������,strSchedulerAddress - ����� ������������
CAgent::CAgent():m_LastCommand( STOP_SCAN )
				,m_CurState( Idling )
{
	Settings::instance().GetParam( SCHEDULER_ADDRESS, m_strSchedulerAddress ); 
	DWORD dwThreadId;
	//��������� ����� �������������(�������� �������� TCP ����������)
	::CloseHandle( ::CreateThread( 0, 0, fnListenThreadProc, this, 0, &dwThreadId ) );
	::InitializeCriticalSection( &m_csCurState );
	::InitializeCriticalSection( &m_csCommandExec );
	//�������������� �������
	m_hCancelEvent = CreateEvent( 0, 1, 0, 0 );
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
	CloseHandle( m_hCancelEvent );
}

//����� ��������� �������� ���������
DWORD WINAPI CAgent::fnProcessThreadProc( LPVOID pParameter )
{
	std::auto_ptr< ProcessParam > pParams( (ProcessParam*)pParameter );
	BYTE bCommandId;
	CPacket Msg;
	try
	{
		//������ ������ ��� ������� ��������� ������
		Msg.SetBuffer( pParams->pbBuf, pParams->iCount );
		for(;;)
		{
				if( !Msg.IsDone() )	
					Msg.GetCommandId( bCommandId );
				else
				{
					Log::instance().Trace( 95, "CAgent::fnProcessThreadProc: ��������� ��������� ������ ���������" ); 
					break;
				}
			(pParams->pThis->*(pParams->pThis->m_mapHandlers[ bCommandId ]))( Msg, pParams->client_sock.get(), pParams->hCancelEvent );
		}
		
		return 0;
	}catch( CPacket::PacketFormatErr )
	{
		Log::instance().Trace( 50, "CAgent::fnProcessThreadProc: ������ ����� ��������� �������" );
		return 0;
	}
	catch( std::exception& e )
	{
		Log::instance().Trace( 50, "CAgent::fnProcessThreadProc: �������� ����������: %s", e.what() );
		return 0;
	}
}

//����� �������� �������� ����������
DWORD WINAPI CAgent::fnListenThreadProc(  void* pParameter )
{
	try{
		CAgent* pThis = (CAgent*)pParameter;
		CServerSocket sock;
		std::auto_ptr< CSocket > client_sock;
		BYTE pBuf[10240];
		int iCount = 0;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CAgent:: ������ ������ �������� �������� ����������" ); 
	    //��������� ��������� ����� � ��������� �������
		sock.Bind( 5000, "172.16.6.53" );
		//��������� ����� � ����� �������������
		sock.Listen();
		//������� �������� ���������� � ������������ ���
		while( NULL != ( client_sock = sock.Accept( adr ) ).get() )
		{
			Log::instance().Trace( 51, "CAgent::ListenThread: �������� ���������� � ������: %s", adr.strAddr.c_str() );
			try{
				//��������� ���������� ������ �� ��������� ������� ������������
				if( ( pThis->m_strSchedulerAddress == adr.strAddr ) && ( ( iCount = client_sock->Receive( pBuf, 10240 ) ) ) )
				{
					ProcessParam* params = new ProcessParam;
					params->client_sock = client_sock;
					params->iCount = iCount;
					params->pbBuf = pBuf;
					params->pThis = pThis;
					Log::instance().Dump( 90, pBuf, iCount, "CAgent::ListenThread: ������������ �����:" );
					CloseHandle( ::CreateThread( 0, 0, fnProcessThreadProc, params, 0, NULL ) );
				}else
					Log::instance().Trace( 50, "CAgent::ListenThread: ������ ����� � ������: %s. ����������" );
			}catch( CSocket::SocketErr )
			{
				Log::instance().Trace( 50, "CAgent::ListenThread: ������ ����� ������� �������� �������" );
				//���� �������� ����� ������ 10��
				continue;
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: �������� ����������: %s", e.what() );
	}
	Log::instance().Trace( 50, "CAgent::ListenThread: ���������� ������ �������� �������� ���������" );
	return 0;
}

//����������� ������
void CAgent::GetStatus( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent )
{
	Log::instance().Trace( 90, "CAgent: �������� ������ �� ��������� �������" );
	//���������� ����� �������
	BYTE pbResp[2];
	pbResp[0] = (BYTE)RESP_OK;
	pbResp[1] = m_CurState;
	pSchedSocket->Send( pbResp, sizeof( pbResp ) );
}
	
void CAgent::StartScan( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent )
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

	//���������� ����� �������, ������� ������� �� ���������
	enumAgentResponse resp = RESP_OK;
	pSchedSocket->Send( (BYTE*)&resp, 1 );
	for( unsigned int i = 0; ( i < dwCount ) && ( WAIT_OBJECT_0 != WaitForSingleObject( hCancelEvent, 0 ) ); i++ )
	{
		//�������� ��������� ����� � ���������� ��� ������������ �� ���� ��������� ����������
		Msg.GetAddress( strAddress );
		for( PluginIterator It = m_PluginContainer.begin(); ( It != m_PluginContainer.end() ) && ( WAIT_OBJECT_0 != WaitForSingleObject( hCancelEvent, 0 ) ); It++ )
		{
			Log::instance().Trace( 80, "CAgent::StartScan: ��������� ����� %s � ������� ������� %s", strAddress.c_str(), (*It)->GetProtocolName() );
			(*It)->Scan( strAddress, List, hCancelEvent );
		}
	}
	::EnterCriticalSection( &m_csCurState );
	m_CurState = Idling;		
	::LeaveCriticalSection( &m_csCurState );
	
	m_LastCommand = START_SCAN;
	::LeaveCriticalSection( &m_csCommandExec );
}

void CAgent::GetData( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent )
{
	Log::instance().Trace( 90, "CAgent: �������� ������ �� ��������� ������" );
}
	
void CAgent::StopScan( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent )
{
	Log::instance().Trace( 90, "CAgent::StopScan: �������� ������ �� ��������� ������������" );
	//���������� ����� �������, ������� ������� �� ���������
	enumAgentResponse resp = RESP_OK;
	pSchedSocket->Send( (BYTE*)&resp, 1 );
	if( m_CurState == Scanning )
	{
		Log::instance().Trace( 90, "CAgent::StopScan: ������������� ������������" ); 
		//������� ������� ������ ���������� ������� � ���� ���������� ������ ���������
		SetEvent( m_hCancelEvent );
	}
}

