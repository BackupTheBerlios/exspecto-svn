//-------------------------------------------------------------------------------------//
//Module: CScheduler class
//Author: Parshin Dmitry
//Description:
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CScheduler.h"
#include "constants.h"
#include "DbProviderFactory.h"

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
{
	int iLogLevel;
	Settings::instance().SetModule( "ScanServer", pServerParamTypes, sizeof( pServerParamTypes )/sizeof( pServerParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );
	DbProviderFactory::instance();

	m_pListenThread = SmartPtr<CThread>( new CThread( SmartPtr<CThreadTask>( new CListenThreadTask( this ) ) ) );
	//TODO:
	Sleep(2000);
	m_bStarted = m_pListenThread->IsWorking();
		

	Log::instance().Trace( 90, "CScheduler:" );
	m_pTrigger = std::auto_ptr< CTimer >( new CTimer( this ) );
	m_pTrigger->Start();
}

CScheduler::~CScheduler(void)
{
	m_CloseEv.Set();
	m_pTrigger->Stop();
	if( m_EventSock.IsConnected() )
	{
		Log::instance().Trace( 90, "CScheduler::~CScheduler: Closing connection" );
		m_EventSock.Close();
	}
	else
		Log::instance().Trace( 90, "CScheduler::~CScheduler: Event connection already closed" );

	Log::instance().Trace( 90, "CScheduler::~CScheduler: " );
}

void CScheduler::OnStartScan()
{
	try{
		m_mtxAgentsContainer.Lock();
		m_mapAgentsContainer.clear();
		std::list< std::string > listAgents;
		Settings::instance().GetParam( "AgentList", listAgents );
		for( std::list< std::string >::iterator It = listAgents.begin(); It != listAgents.end(); It++ )
			m_mapAgentsContainer[ *It ] = SmartPtr< CAgentHandler >( new CAgentHandler( *It ) ); 
		m_mtxAgentsContainer.Unlock();
		for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = m_mapAgentsContainer.begin(); It != m_mapAgentsContainer.end();)
		{
            It->second->Open();
			if( !It->second->IsOpened() )
			{
				Log::instance().Trace( 10, "CScheduler::OnStartScan: Agent %s doesn`t answer", It->first.c_str() );
				m_mtxAgentsContainer.Lock();
				m_mapAgentsContainer.erase( It++ );
				m_mtxAgentsContainer.Lock();
			}else
				It++;
		}
		if( m_mapAgentsContainer.empty() )
		{
			Log::instance().Trace( 10, "CScheduler::OnStartScan: No more agents, stop scan" );
			return;
		}

		std::vector< std::string > vecAdr;
		Settings::instance().GetParam( SCAN_AREA, vecAdr );
		Log::instance().Trace( 12, "CScheduler::OnStartScan: addresses count: %d", vecAdr.size() );
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
			for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItWait = m_mapAgentsContainer.begin(); ItWait != m_mapAgentsContainer.end(); ItWait++ )
			{
				int iPollingInterval;
				Settings::instance().GetParam( POLLING_INTERVAL, iPollingInterval );

				for( int i = 0; i < 1000; i++ )
				  {
					if( m_CloseEv.Wait(0) )
					  {
						Log::instance().Trace( 10, "CScheduler::OnStartScan: Closed" );
						return;
					  }
					else if( ItWait->second->GetScanFinishedEvent().Wait(0) )
					  {
						bWaitTimeout = false;
						break;
					  }
					Sleep( iPollingInterval );
				  }
			}

			if( bWaitTimeout )
			{
				std::vector< std::string > vecFinished;

				std::vector< std::string > vecInaccess;

				for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItPoll = m_mapAgentsContainer.begin(); ItPoll != m_mapAgentsContainer.end();)
				{
					std::string Response;
					std::string Status;
					try{
						Response = ItPoll->second->GetStatus( Status );
					}catch( SocketErr& )
					{
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
					Log::instance().Trace( 10, "CScheduler::OnStartScan: No more agents" );
					break;
				}
				for( std::vector< std::string >::iterator ItInacc = vecInaccess.begin(); ItInacc != vecInaccess.end(); ItInacc++ )
				{
					Log::instance().Trace( 10, "CScheduler::OnStartScan: Inacc ", ItInacc->c_str() );
					if( !vecFinished.empty() )
					{
						Log::instance().Trace( 50, "CScheduler::OnStartScan: " );
						int iPartsCount = mapRanges[ *ItInacc ].size()/vecFinished.size();
						int i = 1, iStartPos = 0, iEndPos = 0;
						for( std::vector< std::string >::iterator ItFinished = vecFinished.begin(); ItFinished != vecFinished.end(); ItFinished++, i++ )
						{
							Log::instance().Trace( 50, "CScheduler::OnStartScan:  %s", ItFinished->c_str() );
							iStartPos = iEndPos;
							iEndPos = i*iPartsCount;
							mapRanges[ *ItFinished ].insert( mapRanges[ *ItFinished ].begin(), mapRanges[ *ItInacc ].begin() + iStartPos, mapRanges[ *ItInacc ].begin() + iEndPos );
							m_mapAgentsContainer[ *ItFinished ]->BeginScan( mapRanges[ *ItFinished ] );
						}
						vecFinished.clear();
					}else
					{
						Log::instance().Trace( 50, "CScheduler::OnStartScan:" );
						int iPartsCount = mapRanges[ *ItInacc ].size()/m_mapAgentsContainer.size();
						int i = 1, iStartPos = 0, iEndPos = 0;
						for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItFinished = m_mapAgentsContainer.begin(); ItFinished != m_mapAgentsContainer.end(); ItFinished++, i++ )
						{
							Log::instance().Trace( 50, "CScheduler::OnStartScan:  %s", ItFinished->first.c_str() );
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
		Log::instance().Trace( 10, "CScheduler::OnStartScan: " );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10, "CScheduler::OnStartScan: e= %s", e.what() );
	}
	catch( ... )
	{
		Log::instance().Trace( 10," CScheduler::OnStartScan: " );
	}
}


void CScheduler::CListenThreadTask::Execute( CEvent& CancelEv )
{
	try{
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CScheduler::fnListenThreadProc: " ); 

		int iEventPort;
		Settings::instance().GetParam( EVENT_PORT, iEventPort );
		m_pScheduler->m_EventSock.Bind( iEventPort );

		m_pScheduler->m_EventSock.Listen();

		while( ( NULL != ( client_sock = m_pScheduler->m_EventSock.Accept( adr ) ).get() ) && !m_pScheduler->m_CloseEv.Wait(0) )
		{
			CLock lock( m_pScheduler->m_mtxAgentsContainer );
			Log::instance().Trace( 51, "CScheduler::ListenThread: : %s", adr.strAddr.c_str() );
			try{

				if( m_pScheduler->m_mapAgentsContainer.find( adr.strAddr ) != m_pScheduler->m_mapAgentsContainer.end() ) 
					m_pScheduler->m_mapAgentsContainer[ adr.strAddr ]->OnConnection( client_sock );
				else
					Log::instance().Trace( 50, "CScheduler::ListenThread: " );
			}catch( SocketErr& e )
			{
				Log::instance().Trace( 50, "CScheduler::ListenThread:  socket: %s", e.what() );
				continue;
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CScheduler::ListenThread: : %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CScheduler::ListenThread: " );
	}
	Log::instance().Trace( 50, "CScheduler::ListenThread: " );
}

