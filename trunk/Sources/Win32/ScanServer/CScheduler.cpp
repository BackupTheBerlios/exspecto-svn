//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CScheduler.h"
#include "constants.h"

//�������� ����� ����������
static char* pServerParamTypes[] = {
	TIMER_VALUE, "int",
	LOG_LEVEL,	"int",
	AGENT_LIST, "string-list",
	SCAN_AREA, "ip-list"
};

CScheduler::CScheduler(void)
{
	int iLogLevel;
	Settings::instance().SetModule( "ScanServer", pServerParamTypes, sizeof( pServerParamTypes )/sizeof( pServerParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );
	
	std::list< std::string > listAgents;
	Settings::instance().GetParam( "AgentList", listAgents );
	//��������� ��������� �������
	for( std::list< std::string >::iterator It = listAgents.begin(); It != listAgents.end(); It++ )
		m_mapAgentsContainer[ *It ] = SmartPtr< CAgentHandler >( new CAgentHandler( *It ) ); 
	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThreadProc, this, 0, NULL );
		
	Log::instance().Trace( 90, "CScheduler: ��������, �������� ������" );
	m_pTrigger = std::auto_ptr< CTimer >( new CTimer( this ) );
	m_pTrigger->Start();
}

CScheduler::~CScheduler(void)
{
	m_pTrigger->Stop();
	m_CloseEv.Set();
	if( m_EventSock.IsConnected() )
	{
		Log::instance().Trace( 90, "CScheduler::~CScheduler: ����� ������� ������" );
		m_EventSock.Close();
	}
	else
		Log::instance().Trace( 90, "CScheduler::~CScheduler: ����� ������� ������" );

	Log::instance().Trace( 90, "CScheduler::~CScheduler: �������� �������� ������ �������������" );
	WaitForSingleObject( m_hListenThread, 10000 );
	Log::instance().Trace( 90, "CScheduler::~CScheduler: 2" );
	CloseHandle( m_hListenThread );
}

void CScheduler::OnStartScan()
{
	try{
		std::vector< std::string > listAdr;
		Settings::instance().GetParam( SCAN_AREA, listAdr );
		int i = 1, iStartPos = 0, iEndPos = 0;
		for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = m_mapAgentsContainer.begin(); It != m_mapAgentsContainer.end(); It++, i++ )
		{
			std::list< std::string > IpList;
			iStartPos = iEndPos;
			int iEndPos = i*(int)listAdr.size()/(int)m_mapAgentsContainer.size();
			
			IpList.insert( IpList.begin(), listAdr.begin() + iStartPos, listAdr.begin() + iEndPos );
			It->second->Open();
			if( It->second->IsOpened() )
				It->second->BeginScan( IpList );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10, "CScheduler::OnStartScan: e= %s", e.what() );
	}
	catch( ... )
	{
		Log::instance().Trace( 10," CScheduler::OnStartScan: �������� ����������� ����������" );
	}
}

//����� �������� �������� ����������
unsigned _stdcall CScheduler::fnListenThreadProc(  void* pParameter )
{
	try{
		CScheduler* pThis = (CScheduler*)pParameter;
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CScheduler::fnListenThreadProc: ������ ������ �������� �������� ����������" ); 
	    //��������� ��������� ����� � ��������� �������
		pThis->m_EventSock.Bind( 3000, "127.0.0.1" );
		//��������� ����� � ����� �������������
		pThis->m_EventSock.Listen();
		//������� �������� ���������� � ������������ ���
		while( ( NULL != ( client_sock = pThis->m_EventSock.Accept( adr ) ).get() ) && ( WAIT_OBJECT_0 != WaitForSingleObject( pThis->m_CloseEv, 0 ) ) )
		{
			Log::instance().Trace( 51, "CScheduler::ListenThread: �������� ���������� � ������: %s", adr.strAddr.c_str() );
			try{
				//��������� ���������� ������ �� ��������� ������� ������������
				if( pThis->m_mapAgentsContainer.find( adr.strAddr ) != pThis->m_mapAgentsContainer.end() ) 
					pThis->m_mapAgentsContainer[ adr.strAddr ]->OnConnection( client_sock );
				else
					Log::instance().Trace( 50, "CScheduler::ListenThread: ������ ����� � ������: %s. ����������" );
			}catch( CSocket::SocketErr& e )
			{
				Log::instance().Trace( 50, "CScheduler::ListenThread: ���������� socket: %s", e.what() );
				//���� �������� ����� ������ 10��
				continue;
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CScheduler::ListenThread: �������� ����������: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CScheduler::ListenThread: �������� ����������� ����������" );
	}
	Log::instance().Trace( 50, "CScheduler::ListenThread: ���������� ������ �������� �������� ���������" );
	return 0;
}
