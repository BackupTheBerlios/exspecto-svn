//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CScheduler.h"
#include "constants.h"
#include "DbProviderFactory.h"

//�������� ����� ����������
static char* pServerParamTypes[] = {
	TIMER_VALUE, "int",
	LOG_LEVEL,	"int",
	AGENT_LIST, "string-list",
	SCAN_AREA, "ip-list",
	DB_PROV_NAME, "string",
	EVENT_PORT, "int",
	AGENT_LISTEN_PORT, "int",
	POLLING_INTERVAL, "int"
};

CScheduler::CScheduler(void):m_bStarted(false)
{
	int iLogLevel;
	Settings::instance().SetModule( "ScanServer", pServerParamTypes, sizeof( pServerParamTypes )/sizeof( pServerParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );
	DbProviderFactory::instance();


	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThreadProc, this, 0, NULL );
	//���� ����� �� �������� � ������� 2 � - ������������� ������ �������
	if( WAIT_TIMEOUT == WaitForSingleObject( m_hListenThread, 2000 ) )
		m_bStarted = true;

	Log::instance().Trace( 90, "CScheduler: ��������, �������� ������" );
	m_pTrigger = std::auto_ptr< CTimer >( new CTimer( this ) );
	m_pTrigger->Start();
}

CScheduler::~CScheduler(void)
{
	m_CloseEv.Set();
	m_pTrigger->Stop();
	if( m_EventSock.IsConnected() )
	{
		Log::instance().Trace( 90, "CScheduler::~CScheduler: ����� ������� ������" );
		m_EventSock.Close();
	}
	else
		Log::instance().Trace( 90, "CScheduler::~CScheduler: ����� ������� ������" );

	Log::instance().Trace( 90, "CScheduler::~CScheduler: �������� �������� ������ �������������" );
	HANDLE hEvents[] = { m_hListenThread };
	WaitForMultipleObjects( sizeof( hEvents )/sizeof( hEvents[0] ), hEvents, TRUE, 10000 );
	CloseHandle( m_hListenThread );
}

void CScheduler::OnStartScan()
{
	try{
		//�������� ����� ������ ������������
		DWORD dwStartScanTime = GetTickCount();		
		m_csAgentsContainer.Enter();
		//��������� ��������� �������
		m_mapAgentsContainer.clear();
		std::list< std::string > listAgents;
		Settings::instance().GetParam( "AgentList", listAgents );
		for( std::list< std::string >::iterator It = listAgents.begin(); It != listAgents.end(); It++ )
			m_mapAgentsContainer[ *It ] = SmartPtr< CAgentHandler >( new CAgentHandler( *It ) ); 
		m_csAgentsContainer.Leave();
		//��������� ���������� � ��������, ��������� �����������
		for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = m_mapAgentsContainer.begin(); It != m_mapAgentsContainer.end();)
		{
            It->second->Open();
			if( !It->second->IsOpened() )
			{
				Log::instance().Trace( 10, "CScheduler::OnStartScan: ����� %s �� ��������, ��������� �� ������ � ������� ������������", It->first.c_str() );
				m_csAgentsContainer.Enter();
				m_mapAgentsContainer.erase( It++ );
				m_csAgentsContainer.Leave();
			}else
				It++;
		}
		if( m_mapAgentsContainer.empty() )
		{
			Log::instance().Trace( 10, "CScheduler::OnStartScan: ��� ��������� �������, �������� ������������" );
			return;
		}
		//������������ �������
		std::vector< std::string > vecAdr;
		Settings::instance().GetParam( SCAN_AREA, vecAdr );
		Log::instance().Trace( 12, "CScheduler::OnStartScan: ����� ������� ��� ������������: %d", vecAdr.size() );
		std::map< std::string, std::vector< std::string > > mapRanges;
		int i = 1, iEndPos = 0, iStartPos = 0;
		std::vector< std::string > vecRange;
		for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = m_mapAgentsContainer.begin(); It != m_mapAgentsContainer.end(); It++, i++ )
		{
			iStartPos = iEndPos;
			iEndPos = i*(int)vecAdr.size()/(int)m_mapAgentsContainer.size();
			vecRange.insert( vecRange.end(), vecAdr.begin() + iStartPos, vecAdr.begin() + iEndPos );
			mapRanges[ It->first ] = vecRange;
			It->second->BeginScan( vecRange );
			vecRange.clear();
		}

		DWORD dwWaitRes;
		HANDLE hEvents[2];
		hEvents[0] = m_CloseEv;
		bool bFail = true;
		while( bFail )
		{
			//������� � ������� ������� ������ ��������� ������������
			for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItWait = m_mapAgentsContainer.begin(); ItWait != m_mapAgentsContainer.end(); ItWait++ )
			{
				int iPollingInterval;
				Settings::instance().GetParam( POLLING_INTERVAL, iPollingInterval );
				hEvents[1] = ItWait->second->GetScanFinishedEvent();
				if( (WAIT_OBJECT_0) == ( dwWaitRes = WaitForMultipleObjects( 2, hEvents, FALSE, iPollingInterval*1000 ) ) )
				{
					Log::instance().Trace( 10, "CScheduler::OnStartScan: ������������ ��������!" );
					return;
				}else if( (WAIT_OBJECT_0+1) == dwWaitRes )
				{
					//Log::instance().Trace( 50, "CScheduler::OnStartScan: ����� %s �������� ������������", ItWait->first.c_str() );
				}else if( WAIT_TIMEOUT == dwWaitRes )
				{
					break;
				}
			}
			//���� �� ��������� ������ �� ������ ���� ������ - ������������ 
			//��� ������� ����� ��� ������������ ������������
			if( WAIT_TIMEOUT == dwWaitRes )
			{
				//������, ����������� ������������
				std::vector< std::string > vecFinished;
				//����������� ������
				std::vector< std::string > vecInaccess;
				//���������� �������
				for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItPoll = m_mapAgentsContainer.begin(); ItPoll != m_mapAgentsContainer.end();)
				{
					enumAgentResponse Response;
					enumAgentState Status;
					try{
						Response = ItPoll->second->GetStatus( Status );
					}catch( SocketErr& )
					{
						//������������ ������ ����� � ������ ��������� �������
						Response = RESP_PROC_ERR;
					}
					if( ( Response == RESP_OK ) && ( Status == Idling ) )
					{
						vecFinished.push_back( ItPoll->first );
						ItPoll++;
					}else if( Response != RESP_OK )
					{
						vecInaccess.push_back( ItPoll->first );
						m_csAgentsContainer.Enter();
						m_mapAgentsContainer.erase( ItPoll++ );
						m_csAgentsContainer.Leave();
					}else
						ItPoll++;
				}
				if( m_mapAgentsContainer.empty() )
				{
					Log::instance().Trace( 10, "CScheduler::OnStartScan: ��������� ������� �� ��������,��������� ������� ������������" );
					break;
				}
				//���� �� ��������� ������ �� ������ ���� ������ - ������������ 
				//��� ������� ����� ��� ������������ ������������
				for( std::vector< std::string >::iterator ItInacc = vecInaccess.begin(); ItInacc != vecInaccess.end(); ItInacc++ )
				{
					Log::instance().Trace( 10, "CScheduler::OnStartScan: ����� %s �� ��������, ��������� �� ������ � ������� ������������", ItInacc->c_str() );
					if( !vecFinished.empty() )
					{
						Log::instance().Trace( 50, "CScheduler::OnStartScan: ��������� ������� ����� ��������,������������ ������������" );
						int iPartsCount = mapRanges[ *ItInacc ].size()/vecFinished.size();
						int i = 1, iStartPos = 0, iEndPos = 0;
						for( std::vector< std::string >::iterator ItFinished = vecFinished.begin(); ItFinished != vecFinished.end(); ItFinished++, i++ )
						{
							Log::instance().Trace( 50, "CScheduler::OnStartScan: ��������� ������� ������ %s", ItFinished->c_str() );
							iStartPos = iEndPos;
							iEndPos = i*iPartsCount;
							mapRanges[ *ItFinished ].insert( mapRanges[ *ItFinished ].begin(), mapRanges[ *ItInacc ].begin() + iStartPos, mapRanges[ *ItInacc ].begin() + iEndPos );
							m_mapAgentsContainer[ *ItFinished ]->BeginScan( mapRanges[ *ItFinished ] );
						}
						vecFinished.clear();
					}else
					{
						Log::instance().Trace( 50, "CScheduler::OnStartScan: ��������� ������� ����� ����������� ��������" );
						int iPartsCount = mapRanges[ *ItInacc ].size()/m_mapAgentsContainer.size();
						int i = 1, iStartPos = 0, iEndPos = 0;
						for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItFinished = m_mapAgentsContainer.begin(); ItFinished != m_mapAgentsContainer.end(); ItFinished++, i++ )
						{
							Log::instance().Trace( 50, "CScheduler::OnStartScan: ��������� ������� ������ %s", ItFinished->first.c_str() );
							iStartPos = iEndPos;
							iEndPos = i*iPartsCount;
							mapRanges[ ItFinished->first ].insert( mapRanges[ ItFinished->first ].begin(), mapRanges[ ItFinished->first ].begin() + iStartPos, mapRanges[ ItFinished->first ].begin() + iEndPos );
							ItFinished->second->BeginScan( mapRanges[ ItFinished->first ] );
						}
					}
				}
			}else
				bFail = false;
		}
		Log::instance().Trace( 10, "CScheduler::OnStartScan: ������������ ��������� �������" );
		Log::instance().Trace( 12, "CScheduler::OnStartScan: ����� ����� ������������: %d", GetTickCount() - dwStartScanTime );
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
		int iEventPort;
		Settings::instance().GetParam( EVENT_PORT, iEventPort );
		pThis->m_EventSock.Bind( iEventPort );
		//��������� ����� � ����� �������������
		pThis->m_EventSock.Listen();
		//������� �������� ���������� � ������������ ���
		while( ( NULL != ( client_sock = pThis->m_EventSock.Accept( adr ) ).get() ) && ( WAIT_OBJECT_0 != WaitForSingleObject( pThis->m_CloseEv, 0 ) ) )
		{
			CLock lock( pThis->m_csAgentsContainer );
			Log::instance().Trace( 51, "CScheduler::ListenThread: �������� ���������� � ������: %s", adr.strAddr.c_str() );
			try{
				//��������� ���������� ������ �� ��������� ������� ������������
				if( pThis->m_mapAgentsContainer.find( adr.strAddr ) != pThis->m_mapAgentsContainer.end() ) 
					pThis->m_mapAgentsContainer[ adr.strAddr ]->OnConnection( client_sock );
				else
					Log::instance().Trace( 50, "CScheduler::ListenThread: ������ ����� � ������: %s. ����������" );
			}catch( SocketErr& e )
			{
				Log::instance().Trace( 50, "CScheduler::ListenThread: ���������� socket: %s", e.what() );
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

