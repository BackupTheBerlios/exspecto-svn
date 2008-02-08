//-------------------------------------------------------------------------------------//
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �������ۜ��М������, �������՜��М��ۜ��؜��ל�����������؜��� ���������ݜ��ڜ�����؜��� ���ߜ��ۜ��М��ݜ��؜������ޜ��Ҝ�����؜��ڜ���
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CScheduler.h"
#include "constants.h"
#include "DbProviderFactory.h"

//�������ߜ��؜�����М��ݜ��؜��� ������؜��ߜ��ޜ��� ���ߜ��М������М��ܜ��՜���������ޜ���
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

CScheduler::CScheduler(void)
 :m_bStarted(false)
 ,m_ListenThread( SmartPtr<CThreadTask>( new CListenThreadTask( this ) ) )
{
	int iLogLevel;
	Settings::instance().SetModule( "ScanServer", pServerParamTypes, sizeof( pServerParamTypes )/sizeof( pServerParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );
	DbProviderFactory::instance();

	//����������ۜ��� ���ߜ��ޜ�����ޜ��� ���ݜ��� ���ל��М��ڜ���������ۜ������ ���� ������՜�����՜��ݜ��؜��� 2 ���� - ���؜��ݜ��؜�����؜��М��ۜ��؜��ל��М�����؜��� ���ߜ������ޜ�����ۜ��� ���������ߜ��՜�����ݜ���
	//TODO:
	Sleep(2000);
	if( m_ListenThread.IsWorking() )
		m_bStarted = true;

	Log::instance().Trace( 90, "CScheduler: ������ޜ��ל��Ԝ��М��ݜ��؜���, ���������М������������՜��� ������М��ٜ��ܜ��՜���" );
	m_pTrigger = std::auto_ptr< CTimer >( new CTimer( this ) );
	m_pTrigger->Start();
}

CScheduler::~CScheduler(void)
{
	m_CloseEv.Set();
	m_pTrigger->Stop();
	if( m_EventSock.IsConnected() )
	{
		Log::instance().Trace( 90, "CScheduler::~CScheduler: �������М��ݜ��М��� ������ޜ��ќ��������؜��� ���ߜ��ޜ��Ԝ��ݜ������" );
		m_EventSock.Close();
	}
	else
		Log::instance().Trace( 90, "CScheduler::~CScheduler: �������М��ݜ��М��� ������ޜ��ќ��������؜��� ���ޜ��ߜ��������՜���" );

	Log::instance().Trace( 90, "CScheduler::~CScheduler: �������֜��؜��Ԝ��М��ݜ��؜��� ���ל��М��ڜ������������؜��� ���ߜ��ޜ�����ޜ��ڜ��� ���ߜ������ޜ�����ۜ��������؜��Ҝ��М��ݜ��؜���" );
}

void CScheduler::OnStartScan()
{
	try{
		//�������М�����՜��ڜ��М��՜��� ���Ҝ������՜��ܜ��� ���ݜ��М�����М��ۜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���
		m_mtxAgentsContainer.Lock();
		//�������М��Ӝ���������֜��М��՜��� ���ڜ��ޜ��ݜ�����՜��ٜ��ݜ��՜��� ���М��Ӝ��՜��ݜ�����ޜ���
		m_mapAgentsContainer.clear();
		std::list< std::string > listAgents;
		Settings::instance().GetParam( "AgentList", listAgents );
		for( std::list< std::string >::iterator It = listAgents.begin(); It != listAgents.end(); It++ )
			m_mapAgentsContainer[ *It ] = SmartPtr< CAgentHandler >( new CAgentHandler( *It ) ); 
		m_mtxAgentsContainer.Unlock();
		//����������ڜ���������Ҝ��М��՜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� ���� ���М��Ӝ��՜��ݜ�����М��ܜ���, ���ޜ��������՜��؜��Ҝ��М��՜��� ���ݜ��՜��Ԝ��ޜ�����������ߜ��ݜ������
		for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = m_mapAgentsContainer.begin(); It != m_mapAgentsContainer.end();)
		{
            It->second->Open();
			if( !It->second->IsOpened() )
			{
				Log::instance().Trace( 10, "CScheduler::OnStartScan: �������Ӝ��՜��ݜ��� %s ���ݜ��� ���Ԝ��ޜ�����������ߜ��՜���, ���؜�����ڜ��ۜ��������М��՜��� ���؜��� ������ߜ��؜�����ڜ��� ���� ������՜��ڜ��������՜��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���", It->first.c_str() );
				m_mtxAgentsContainer.Lock();
				m_mapAgentsContainer.erase( It++ );
				m_mtxAgentsContainer.Lock();
			}else
				It++;
		}
		if( m_mapAgentsContainer.empty() )
		{
			Log::instance().Trace( 10, "CScheduler::OnStartScan: �������՜��� ���Ԝ��ޜ�����������ߜ��ݜ������ ���М��Ӝ��՜��ݜ�����ޜ���, ���ޜ�����ܜ��՜��ݜ�����՜��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���" );
			return;
		}
		//�������М�����ߜ������՜��Ԝ��՜��ۜ�����՜��� ���ל��М��Ԝ��М��ݜ��؜���
		std::vector< std::string > vecAdr;
		Settings::instance().GetParam( SCAN_AREA, vecAdr );
		Log::instance().Trace( 12, "CScheduler::OnStartScan: ����������՜��Ӝ��� ���М��Ԝ������՜�����ޜ��� ���Ԝ��ۜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���: %d", vecAdr.size() );
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

		bool bFail = true;
		bool bWaitTimeout;
		while( bFail )
		{
		  bWaitTimeout = true;
			//�������֜��؜��Ԝ��М��՜��� ���� ������՜�����՜��ݜ��؜��� ���ߜ��՜������؜��ޜ��Ԝ��� ���ޜ��ߜ������ޜ������ ���ޜ��ڜ��ޜ��ݜ�����М��ݜ��؜��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���
			for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItWait = m_mapAgentsContainer.begin(); ItWait != m_mapAgentsContainer.end(); ItWait++ )
			{
				int iPollingInterval;
				Settings::instance().GetParam( POLLING_INTERVAL, iPollingInterval );

				for( int i = 0; i < 1000; i++ )
				  {
					if( m_CloseEv.Wait(0) )
					  {
						Log::instance().Trace( 10, "CScheduler::OnStartScan: �������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ޜ�����ܜ��՜��ݜ��՜��ݜ���!" );
						return;
					  }
					else if( ItWait->second->GetScanFinishedEvent().Wait(0) )
					  {
						bWaitTimeout = false;
						break;
						//Log::instance().Trace( 50, "CScheduler::OnStartScan: �������Ӝ��՜��ݜ��� %s ���ל��М��ڜ��ޜ��ݜ�����؜��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���", ItWait->first.c_str() );
					  }
					Sleep( iPollingInterval );
				  }
			}
			//����������ۜ��� ���ݜ��� ���Ԝ��ޜ��֜��Ԝ��М��ۜ��؜������ ���ޜ�����Ҝ��՜������ ���ޜ��� ���ڜ��М��ڜ��ޜ��Ӝ��� ���ۜ��؜��ќ��� ���М��Ӝ��՜��ݜ������ - �������М�����ߜ������՜��Ԝ��՜��ۜ�����՜��� 
			//���՜��Ӝ��� ���ל��М��Ԝ��М��ݜ��؜��� ���ܜ��՜��֜��Ԝ��� ������֜��� ���ל��М��ڜ��ޜ��ݜ�����؜��Ҝ�����؜��ܜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���
			if( bWaitTimeout )
			{
				//�������Ӝ��՜��ݜ������, ���ל��М��ڜ��ޜ��ݜ�����؜��Ҝ�����؜��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���
				std::vector< std::string > vecFinished;
				//�������՜��Ԝ��ޜ�����������ߜ��ݜ������ ���М��Ӝ��՜��ݜ������
				std::vector< std::string > vecInaccess;
				//�������ߜ������М�����؜��Ҝ��М��՜��� ���М��Ӝ��՜��ݜ�����ޜ���
				for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItPoll = m_mapAgentsContainer.begin(); ItPoll != m_mapAgentsContainer.end();)
				{
					std::string Response;
					std::string Status;
					try{
						Response = ItPoll->second->GetStatus( Status );
					}catch( SocketErr& )
					{
						//�����������؜������М��Ҝ��ݜ��؜��Ҝ��М��՜��� ���ޜ�����؜��ќ��ڜ��� ������Ҝ�����ל��� ���� ���ޜ�����؜��ќ��ڜ��� ���ޜ��ќ������М��ќ��ޜ�����ڜ��� ���ڜ��ޜ��ܜ��М��ݜ��Ԝ���
						Response = RESP_PROC_ERR;
					}
					if( ( Response == AGENT_RESP_OK ) && ( Status == IDLING ) )
					{
						vecFinished.push_back( ItPoll->first );
						ItPoll++;
					}else if( Response != AGENT_RESP_OK )
					{
						vecInaccess.push_back( ItPoll->first );
						m_mtxAgentsContainer.Lock();
						m_mapAgentsContainer.erase( ItPoll++ );
						m_mtxAgentsContainer.Unlock();
					}else
						ItPoll++;
				}
				if( m_mapAgentsContainer.empty() )
				{
					Log::instance().Trace( 10, "CScheduler::OnStartScan: �������ޜ�����������ߜ��ݜ������ ���М��Ӝ��՜��ݜ�����ޜ��� ���ݜ��� ���ޜ��������М��ۜ��ޜ������,���ל��М��Ҝ��՜���������М��՜��� ������՜��ڜ��������՜��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���" );
					break;
				}
				//����������ۜ��� ���ݜ��� ���Ԝ��ޜ��֜��Ԝ��М��ۜ��؜������ ���ޜ�����Ҝ��՜������ ���ޜ��� ���ڜ��М��ڜ��ޜ��Ӝ��� ���ۜ��؜��ќ��� ���М��Ӝ��՜��ݜ������ - �������М�����ߜ������՜��Ԝ��՜��ۜ�����՜��� 
				//���՜��Ӝ��� ���ל��М��Ԝ��М��ݜ��؜��� ���ܜ��՜��֜��Ԝ��� ������֜��� ���ל��М��ڜ��ޜ��ݜ�����؜��Ҝ�����؜��ܜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���
				for( std::vector< std::string >::iterator ItInacc = vecInaccess.begin(); ItInacc != vecInaccess.end(); ItInacc++ )
				{
					Log::instance().Trace( 10, "CScheduler::OnStartScan: �������Ӝ��՜��ݜ��� %s ���ݜ��� ���Ԝ��ޜ�����������ߜ��՜���, ���؜�����ڜ��ۜ��������М��՜��� ���؜��� ������ߜ��؜�����ڜ��� ���� ������՜��ڜ��������՜��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���", ItInacc->c_str() );
					if( !vecFinished.empty() )
					{
						Log::instance().Trace( 50, "CScheduler::OnStartScan: �������М��ל��Ԝ��՜��ۜ�����՜��� ���ל��М��Ԝ��М��ݜ��؜��� ���ܜ��՜��֜��Ԝ��� ���М��Ӝ��՜��ݜ�����М��ܜ���,���ל��М��ڜ��ޜ��ݜ�����؜��Ҝ�����؜��ܜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���" );
						int iPartsCount = mapRanges[ *ItInacc ].size()/vecFinished.size();
						int i = 1, iStartPos = 0, iEndPos = 0;
						for( std::vector< std::string >::iterator ItFinished = vecFinished.begin(); ItFinished != vecFinished.end(); ItFinished++, i++ )
						{
							Log::instance().Trace( 50, "CScheduler::OnStartScan: �������ޜ��ќ��М��Ҝ��ۜ�����՜��� ���ל��М��Ԝ��М��ݜ��؜��� ���М��Ӝ��՜��ݜ������ %s", ItFinished->c_str() );
							iStartPos = iEndPos;
							iEndPos = i*iPartsCount;
							mapRanges[ *ItFinished ].insert( mapRanges[ *ItFinished ].begin(), mapRanges[ *ItInacc ].begin() + iStartPos, mapRanges[ *ItInacc ].begin() + iEndPos );
							m_mapAgentsContainer[ *ItFinished ]->BeginScan( mapRanges[ *ItFinished ] );
						}
						vecFinished.clear();
					}else
					{
						Log::instance().Trace( 50, "CScheduler::OnStartScan: �������М��ל��Ԝ��՜��ۜ�����՜��� ���ל��М��Ԝ��М��ݜ��؜��� ���ܜ��՜��֜��Ԝ��� ���ޜ��������М��Ҝ�����؜��ܜ��؜������ ���М��Ӝ��՜��ݜ�����М��ܜ���" );
						int iPartsCount = mapRanges[ *ItInacc ].size()/m_mapAgentsContainer.size();
						int i = 1, iStartPos = 0, iEndPos = 0;
						for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItFinished = m_mapAgentsContainer.begin(); ItFinished != m_mapAgentsContainer.end(); ItFinished++, i++ )
						{
							Log::instance().Trace( 50, "CScheduler::OnStartScan: �������ޜ��ќ��М��Ҝ��ۜ�����՜��� ���ל��М��Ԝ��М��ݜ��؜��� ���М��Ӝ��՜��ݜ������ %s", ItFinished->first.c_str() );
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
		Log::instance().Trace( 10, "CScheduler::OnStartScan: �������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ל��М��ڜ��ޜ��ݜ�����՜��ݜ��� ���������ߜ��՜�����ݜ���" );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10, "CScheduler::OnStartScan: e= %s", e.what() );
	}
	catch( ... )
	{
		Log::instance().Trace( 10," CScheduler::OnStartScan: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���" );
	}
}


void CScheduler::CListenThreadTask::Execute( CEvent& CancelEv )
{
	try{
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CScheduler::fnListenThreadProc: �������М��ߜ��������� ���ߜ��ޜ�����ޜ��ڜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� ���Ҝ�����ޜ��Ԝ��������؜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜���" ); 
	    //������Ҝ�����ל�����Ҝ��М��՜��� ������՜������Ҝ��՜������ݜ������ ������ޜ��ڜ��՜��� ���� ���ۜ��ޜ��ڜ��М��ۜ�����ݜ������ ���М��Ԝ������՜�����ޜ���
		int iEventPort;
		Settings::instance().GetParam( EVENT_PORT, iEventPort );
		m_pScheduler->m_EventSock.Bind( iEventPort );
		//���ߜ��՜������՜��Ҝ��ޜ��Ԝ��؜��� ������ޜ��ڜ��՜��� ���� �������՜��֜��؜��� ���ߜ������ޜ�����ۜ��������؜��Ҝ��М��ݜ��؜���
		m_pScheduler->m_EventSock.Listen();
		//�������֜��؜��Ԝ��М��՜��� ���Ҝ�����ޜ��Ԝ��������՜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� ���� ���ޜ��ќ������М��ќ��М��������Ҝ��М��՜��� ���՜��Ӝ���
		while( ( NULL != ( client_sock = m_pScheduler->m_EventSock.Accept( adr ) ).get() ) && !m_pScheduler->m_CloseEv.Wait(0) )
		{
			CLock lock( m_pScheduler->m_mtxAgentsContainer );
			Log::instance().Trace( 51, "CScheduler::ListenThread: ����������ޜ��Ԝ��������՜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� ���� ���М��Ԝ������՜������: %s", adr.strAddr.c_str() );
			try{
				//���ߜ������؜��ݜ��؜��ܜ��М��՜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� ������ޜ��ۜ�����ڜ��� ���ޜ��� ���ל��М��Ԝ��М��ݜ��ݜ��ޜ��Ӝ��� ������՜������Ҝ��՜������� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���
				if( m_pScheduler->m_mapAgentsContainer.find( adr.strAddr ) != m_pScheduler->m_mapAgentsContainer.end() ) 
					m_pScheduler->m_mapAgentsContainer[ adr.strAddr ]->OnConnection( client_sock );
				else
					Log::instance().Trace( 50, "CScheduler::ListenThread: �����������؜�����ۜ��� ���ߜ��М��ڜ��՜��� ���� ���М��Ԝ������՜������: %s. �������Ӝ��ݜ��ޜ������؜���������՜���" );
			}catch( SocketErr& e )
			{
				Log::instance().Trace( 50, "CScheduler::ListenThread: ����������ڜ��ۜ��������՜��ݜ��؜��� socket: %s", e.what() );
				continue;
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CScheduler::ListenThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CScheduler::ListenThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���" );
	}
	Log::instance().Trace( 50, "CScheduler::ListenThread: �������М��Ҝ��՜���������՜��ݜ��؜��� ���ߜ��ޜ�����ޜ��ڜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� ���Ҝ�����ޜ��Ԝ��������؜��� ������ޜ��ޜ��ќ�����՜��ݜ��؜���" );
}

