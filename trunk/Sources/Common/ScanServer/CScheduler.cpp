//-------------------------------------------------------------------------------------//
//œô¨Íœô¨âœô¨Þœô¨â œô¨äœô¨Ðœô¨Ùœô¨Û œô¨ïœô¨Òœô¨Ûœô¨ïœô¨Õœô¨âœô¨áœô¨ï œô¨çœô¨Ðœô¨áœô¨âœô¨ìœô¨î œô¨ßœô¨àœô¨Þœô¨Õœô¨Úœô¨âœô¨Ð Exspecto 2006œô¨Ó.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: œô¨ºœô¨Ûœô¨Ðœô¨áœô¨á, œô¨àœô¨Õœô¨Ðœô¨Ûœô¨Øœô¨×œô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨Ø œô¨ßœô¨Ûœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨éœô¨Øœô¨Úœô¨Ð
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CScheduler.h"
#include "constants.h"
#include "DbProviderFactory.h"

//œô¨¾œô¨ßœô¨Øœô¨áœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨âœô¨Øœô¨ßœô¨Þœô¨Ò œô¨ßœô¨Ðœô¨àœô¨Ðœô¨Üœô¨Õœô¨âœô¨àœô¨Þœô¨Ò
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

	//œô¨µœô¨áœô¨Ûœô¨Ø œô¨ßœô¨Þœô¨âœô¨Þœô¨Ú œô¨Ýœô¨Õ œô¨×œô¨Ðœô¨Úœô¨àœô¨ëœô¨Ûœô¨áœô¨ï œô¨Ò œô¨âœô¨Õœô¨çœô¨Õœô¨Ýœô¨Øœô¨Ø 2 œô¨á - œô¨Øœô¨Ýœô¨Øœô¨æœô¨Øœô¨Ðœô¨Ûœô¨Øœô¨×œô¨Ðœô¨æœô¨Øœô¨ï œô¨ßœô¨àœô¨Þœô¨èœô¨Ûœô¨Ð œô¨ãœô¨áœô¨ßœô¨Õœô¨èœô¨Ýœô¨Þ
	//TODO:
	Sleep(2000);
	if( m_ListenThread.IsWorking() )
		m_bStarted = true;

	Log::instance().Trace( 90, "CScheduler: œô¨áœô¨Þœô¨×œô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ, œô¨áœô¨âœô¨Ðœô¨àœô¨âœô¨ãœô¨Õœô¨Ü œô¨âœô¨Ðœô¨Ùœô¨Üœô¨Õœô¨à" );
	m_pTrigger = std::auto_ptr< CTimer >( new CTimer( this ) );
	m_pTrigger->Start();
}

CScheduler::~CScheduler(void)
{
	m_CloseEv.Set();
	m_pTrigger->Stop();
	if( m_EventSock.IsConnected() )
	{
		Log::instance().Trace( 90, "CScheduler::~CScheduler: œô¨ºœô¨Ðœô¨Ýœô¨Ðœô¨Û œô¨áœô¨Þœô¨Ñœô¨ëœô¨âœô¨Øœô¨Ù œô¨ßœô¨Þœô¨Ôœô¨Ýœô¨ïœô¨â" );
		m_EventSock.Close();
	}
	else
		Log::instance().Trace( 90, "CScheduler::~CScheduler: œô¨ºœô¨Ðœô¨Ýœô¨Ðœô¨Û œô¨áœô¨Þœô¨Ñœô¨ëœô¨âœô¨Øœô¨Ù œô¨Þœô¨ßœô¨ãœô¨éœô¨Õœô¨Ý" );

	Log::instance().Trace( 90, "CScheduler::~CScheduler: œô¨¾œô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨×œô¨Ðœô¨Úœô¨àœô¨ëœô¨âœô¨Øœô¨ï œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Ð œô¨ßœô¨àœô¨Þœô¨áœô¨Ûœô¨ãœô¨èœô¨Øœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï" );
}

void CScheduler::OnStartScan()
{
	try{
		//œô¨·œô¨Ðœô¨áœô¨Õœô¨Úœô¨Ðœô¨Õœô¨Ü œô¨Òœô¨àœô¨Õœô¨Üœô¨ï œô¨Ýœô¨Ðœô¨çœô¨Ðœô¨Ûœô¨Ð œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï
		m_mtxAgentsContainer.Lock();
		//œô¨·œô¨Ðœô¨Óœô¨àœô¨ãœô¨Öœô¨Ðœô¨Õœô¨Ü œô¨Úœô¨Þœô¨Ýœô¨âœô¨Õœô¨Ùœô¨Ýœô¨Õœô¨à œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Þœô¨Ò
		m_mapAgentsContainer.clear();
		std::list< std::string > listAgents;
		Settings::instance().GetParam( "AgentList", listAgents );
		for( std::list< std::string >::iterator It = listAgents.begin(); It != listAgents.end(); It++ )
			m_mapAgentsContainer[ *It ] = SmartPtr< CAgentHandler >( new CAgentHandler( *It ) ); 
		m_mtxAgentsContainer.Unlock();
		//œô¨¾œô¨âœô¨Úœô¨àœô¨ëœô¨Òœô¨Ðœô¨Õœô¨Ü œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨á œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Ðœô¨Üœô¨Ø, œô¨Þœô¨âœô¨áœô¨Õœô¨Øœô¨Òœô¨Ðœô¨Õœô¨Ü œô¨Ýœô¨Õœô¨Ôœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Ýœô¨ëœô¨å
		for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = m_mapAgentsContainer.begin(); It != m_mapAgentsContainer.end();)
		{
            It->second->Open();
			if( !It->second->IsOpened() )
			{
				Log::instance().Trace( 10, "CScheduler::OnStartScan: œô¨°œô¨Óœô¨Õœô¨Ýœô¨â %s œô¨Ýœô¨Õ œô¨Ôœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Õœô¨Ý, œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Ðœô¨Õœô¨Ü œô¨Øœô¨× œô¨áœô¨ßœô¨Øœô¨áœô¨Úœô¨Ð œô¨Ò œô¨âœô¨Õœô¨Úœô¨ãœô¨éœô¨Õœô¨Ü œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Ø", It->first.c_str() );
				m_mtxAgentsContainer.Lock();
				m_mapAgentsContainer.erase( It++ );
				m_mtxAgentsContainer.Lock();
			}else
				It++;
		}
		if( m_mapAgentsContainer.empty() )
		{
			Log::instance().Trace( 10, "CScheduler::OnStartScan: œô¨½œô¨Õœô¨â œô¨Ôœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Ýœô¨ëœô¨å œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Þœô¨Ò, œô¨Þœô¨âœô¨Üœô¨Õœô¨Ýœô¨ïœô¨Õœô¨Ü œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ" );
			return;
		}
		//œô¨Àœô¨Ðœô¨áœô¨ßœô¨àœô¨Õœô¨Ôœô¨Õœô¨Ûœô¨ïœô¨Õœô¨Ü œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï
		std::vector< std::string > vecAdr;
		Settings::instance().GetParam( SCAN_AREA, vecAdr );
		Log::instance().Trace( 12, "CScheduler::OnStartScan: œô¨²œô¨áœô¨Õœô¨Óœô¨Þ œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Þœô¨Ò œô¨Ôœô¨Ûœô¨ï œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï: %d", vecAdr.size() );
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
			//œô¨¾œô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Õœô¨Ü œô¨Ò œô¨âœô¨Õœô¨çœô¨Õœô¨Ýœô¨Øœô¨Ø œô¨ßœô¨Õœô¨àœô¨Øœô¨Þœô¨Ôœô¨Ð œô¨Þœô¨ßœô¨àœô¨Þœô¨áœô¨Ð œô¨Þœô¨Úœô¨Þœô¨Ýœô¨çœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï
			for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItWait = m_mapAgentsContainer.begin(); ItWait != m_mapAgentsContainer.end(); ItWait++ )
			{
				int iPollingInterval;
				Settings::instance().GetParam( POLLING_INTERVAL, iPollingInterval );

				for( int i = 0; i < 1000; i++ )
				  {
					if( m_CloseEv.Wait(0) )
					  {
						Log::instance().Trace( 10, "CScheduler::OnStartScan: œô¨Áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨Þœô¨âœô¨Üœô¨Õœô¨Ýœô¨Õœô¨Ýœô¨Þ!" );
						return;
					  }
					else if( ItWait->second->GetScanFinishedEvent().Wait(0) )
					  {
						bWaitTimeout = false;
						break;
						//Log::instance().Trace( 50, "CScheduler::OnStartScan: œô¨°œô¨Óœô¨Õœô¨Ýœô¨â %s œô¨×œô¨Ðœô¨Úœô¨Þœô¨Ýœô¨çœô¨Øœô¨Û œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ", ItWait->first.c_str() );
					  }
					Sleep( iPollingInterval );
				  }
			}
			//œô¨µœô¨áœô¨Ûœô¨Ø œô¨Ýœô¨Õ œô¨Ôœô¨Þœô¨Öœô¨Ôœô¨Ðœô¨Ûœô¨Øœô¨áœô¨ì œô¨Þœô¨âœô¨Òœô¨Õœô¨âœô¨Ð œô¨Þœô¨â œô¨Úœô¨Ðœô¨Úœô¨Þœô¨Óœô¨Þ œô¨Ûœô¨Øœô¨Ñœô¨Þ œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Ð - œô¨àœô¨Ðœô¨áœô¨ßœô¨àœô¨Õœô¨Ôœô¨Õœô¨Ûœô¨ïœô¨Õœô¨Ü 
			//œô¨Õœô¨Óœô¨Þ œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨Üœô¨Õœô¨Öœô¨Ôœô¨ã œô¨ãœô¨Öœô¨Õ œô¨×œô¨Ðœô¨Úœô¨Þœô¨Ýœô¨çœô¨Øœô¨Òœô¨èœô¨Øœô¨Üœô¨Ø œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ
			if( bWaitTimeout )
			{
				//œô¨°œô¨Óœô¨Õœô¨Ýœô¨âœô¨ë, œô¨×œô¨Ðœô¨Úœô¨Þœô¨Ýœô¨çœô¨Øœô¨Òœô¨èœô¨Øœô¨Õ œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ
				std::vector< std::string > vecFinished;
				//œô¨½œô¨Õœô¨Ôœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Ýœô¨ëœô¨Õ œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨ë
				std::vector< std::string > vecInaccess;
				//œô¨¾œô¨ßœô¨àœô¨Ðœô¨èœô¨Øœô¨Òœô¨Ðœô¨Õœô¨Ü œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Þœô¨Ò
				for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItPoll = m_mapAgentsContainer.begin(); ItPoll != m_mapAgentsContainer.end();)
				{
					std::string Response;
					std::string Status;
					try{
						Response = ItPoll->second->GetStatus( Status );
					}catch( SocketErr& )
					{
						//œô¨¿œô¨àœô¨Øœô¨àœô¨Ðœô¨Òœô¨Ýœô¨Øœô¨Òœô¨Ðœô¨Õœô¨Ü œô¨Þœô¨èœô¨Øœô¨Ñœô¨Úœô¨ã œô¨áœô¨Òœô¨ïœô¨×œô¨Ø œô¨Ú œô¨Þœô¨èœô¨Øœô¨Ñœô¨Úœô¨Õ œô¨Þœô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨Úœô¨Ø œô¨Úœô¨Þœô¨Üœô¨Ðœô¨Ýœô¨Ôœô¨ë
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
					Log::instance().Trace( 10, "CScheduler::OnStartScan: œô¨´œô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Ýœô¨ëœô¨å œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Þœô¨Ò œô¨Ýœô¨Õ œô¨Þœô¨áœô¨âœô¨Ðœô¨Ûœô¨Þœô¨áœô¨ì,œô¨×œô¨Ðœô¨Òœô¨Õœô¨àœô¨èœô¨Ðœô¨Õœô¨Ü œô¨âœô¨Õœô¨Úœô¨ãœô¨éœô¨Õœô¨Õ œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ" );
					break;
				}
				//œô¨µœô¨áœô¨Ûœô¨Ø œô¨Ýœô¨Õ œô¨Ôœô¨Þœô¨Öœô¨Ôœô¨Ðœô¨Ûœô¨Øœô¨áœô¨ì œô¨Þœô¨âœô¨Òœô¨Õœô¨âœô¨Ð œô¨Þœô¨â œô¨Úœô¨Ðœô¨Úœô¨Þœô¨Óœô¨Þ œô¨Ûœô¨Øœô¨Ñœô¨Þ œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Ð - œô¨àœô¨Ðœô¨áœô¨ßœô¨àœô¨Õœô¨Ôœô¨Õœô¨Ûœô¨ïœô¨Õœô¨Ü 
				//œô¨Õœô¨Óœô¨Þ œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨Üœô¨Õœô¨Öœô¨Ôœô¨ã œô¨ãœô¨Öœô¨Õ œô¨×œô¨Ðœô¨Úœô¨Þœô¨Ýœô¨çœô¨Øœô¨Òœô¨èœô¨Øœô¨Üœô¨Ø œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ
				for( std::vector< std::string >::iterator ItInacc = vecInaccess.begin(); ItInacc != vecInaccess.end(); ItInacc++ )
				{
					Log::instance().Trace( 10, "CScheduler::OnStartScan: œô¨°œô¨Óœô¨Õœô¨Ýœô¨â %s œô¨Ýœô¨Õ œô¨Ôœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Õœô¨Ý, œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Ðœô¨Õœô¨Ü œô¨Øœô¨× œô¨áœô¨ßœô¨Øœô¨áœô¨Úœô¨Ð œô¨Ò œô¨âœô¨Õœô¨Úœô¨ãœô¨éœô¨Õœô¨Ü œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Ø", ItInacc->c_str() );
					if( !vecFinished.empty() )
					{
						Log::instance().Trace( 50, "CScheduler::OnStartScan: œô¨Àœô¨Ðœô¨×œô¨Ôœô¨Õœô¨Ûœô¨ïœô¨Õœô¨Ü œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨Üœô¨Õœô¨Öœô¨Ôœô¨ã œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Ðœô¨Üœô¨Ø,œô¨×œô¨Ðœô¨Úœô¨Þœô¨Ýœô¨çœô¨Øœô¨Òœô¨èœô¨Øœô¨Üœô¨Ø œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ" );
						int iPartsCount = mapRanges[ *ItInacc ].size()/vecFinished.size();
						int i = 1, iStartPos = 0, iEndPos = 0;
						for( std::vector< std::string >::iterator ItFinished = vecFinished.begin(); ItFinished != vecFinished.end(); ItFinished++, i++ )
						{
							Log::instance().Trace( 50, "CScheduler::OnStartScan: œô¨´œô¨Þœô¨Ñœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨Õœô¨Ü œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨ã %s", ItFinished->c_str() );
							iStartPos = iEndPos;
							iEndPos = i*iPartsCount;
							mapRanges[ *ItFinished ].insert( mapRanges[ *ItFinished ].begin(), mapRanges[ *ItInacc ].begin() + iStartPos, mapRanges[ *ItInacc ].begin() + iEndPos );
							m_mapAgentsContainer[ *ItFinished ]->BeginScan( mapRanges[ *ItFinished ] );
						}
						vecFinished.clear();
					}else
					{
						Log::instance().Trace( 50, "CScheduler::OnStartScan: œô¨Àœô¨Ðœô¨×œô¨Ôœô¨Õœô¨Ûœô¨ïœô¨Õœô¨Ü œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨Üœô¨Õœô¨Öœô¨Ôœô¨ã œô¨Þœô¨áœô¨âœô¨Ðœô¨Òœô¨èœô¨Øœô¨Üœô¨Øœô¨áœô¨ï œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Ðœô¨Üœô¨Ø" );
						int iPartsCount = mapRanges[ *ItInacc ].size()/m_mapAgentsContainer.size();
						int i = 1, iStartPos = 0, iEndPos = 0;
						for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator ItFinished = m_mapAgentsContainer.begin(); ItFinished != m_mapAgentsContainer.end(); ItFinished++, i++ )
						{
							Log::instance().Trace( 50, "CScheduler::OnStartScan: œô¨´œô¨Þœô¨Ñœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨Õœô¨Ü œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨ã %s", ItFinished->first.c_str() );
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
		Log::instance().Trace( 10, "CScheduler::OnStartScan: œô¨Áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨×œô¨Ðœô¨Úœô¨Þœô¨Ýœô¨çœô¨Õœô¨Ýœô¨Þ œô¨ãœô¨áœô¨ßœô¨Õœô¨èœô¨Ýœô¨Þ" );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10, "CScheduler::OnStartScan: e= %s", e.what() );
	}
	catch( ... )
	{
		Log::instance().Trace( 10," CScheduler::OnStartScan: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ" );
	}
}


void CScheduler::CListenThreadTask::Execute( CEvent& CancelEv )
{
	try{
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CScheduler::fnListenThreadProc: œô¨·œô¨Ðœô¨ßœô¨ãœô¨áœô¨Ú œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Ð œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Øœô¨å œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Ù" ); 
	    //œô¨áœô¨Òœô¨ïœô¨×œô¨ëœô¨Òœô¨Ðœô¨Õœô¨Ü œô¨áœô¨Õœô¨àœô¨Òœô¨Õœô¨àœô¨Ýœô¨ëœô¨Ù œô¨áœô¨Þœô¨Úœô¨Õœô¨â œô¨á œô¨Ûœô¨Þœô¨Úœô¨Ðœô¨Ûœô¨ìœô¨Ýœô¨ëœô¨Ü œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Þœô¨Ü
		int iEventPort;
		Settings::instance().GetParam( EVENT_PORT, iEventPort );
		m_pScheduler->m_EventSock.Bind( iEventPort );
		//œô¨ßœô¨Õœô¨àœô¨Õœô¨Òœô¨Þœô¨Ôœô¨Øœô¨Ü œô¨áœô¨Þœô¨Úœô¨Õœô¨â œô¨Ò œô¨àœô¨Õœô¨Öœô¨Øœô¨Ü œô¨ßœô¨àœô¨Þœô¨áœô¨Ûœô¨ãœô¨èœô¨Øœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï
		m_pScheduler->m_EventSock.Listen();
		//œô¨¾œô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Õœô¨Ü œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Õœô¨Õ œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Ø œô¨Þœô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Ðœô¨âœô¨ëœô¨Òœô¨Ðœô¨Õœô¨Ü œô¨Õœô¨Óœô¨Þ
		while( ( NULL != ( client_sock = m_pScheduler->m_EventSock.Accept( adr ) ).get() ) && !m_pScheduler->m_CloseEv.Wait(0) )
		{
			CLock lock( m_pScheduler->m_mtxAgentsContainer );
			Log::instance().Trace( 51, "CScheduler::ListenThread: œô¨²œô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Õœô¨Õ œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨á œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð: %s", adr.strAddr.c_str() );
			try{
				//œô¨ßœô¨àœô¨Øœô¨Ýœô¨Øœô¨Üœô¨Ðœô¨Õœô¨Ü œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨ï œô¨âœô¨Þœô¨Ûœô¨ìœô¨Úœô¨Þ œô¨Þœô¨â œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨Þœô¨Óœô¨Þ œô¨áœô¨Õœô¨àœô¨Òœô¨Õœô¨àœô¨Ð œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï
				if( m_pScheduler->m_mapAgentsContainer.find( adr.strAddr ) != m_pScheduler->m_mapAgentsContainer.end() ) 
					m_pScheduler->m_mapAgentsContainer[ adr.strAddr ]->OnConnection( client_sock );
				else
					Log::instance().Trace( 50, "CScheduler::ListenThread: œô¨¿œô¨àœô¨Øœô¨èœô¨Ûœô¨Õ œô¨ßœô¨Ðœô¨Úœô¨Õœô¨â œô¨á œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð: %s. œô¨¸œô¨Óœô¨Ýœô¨Þœô¨àœô¨Øœô¨àœô¨ãœô¨Õœô¨Ü" );
			}catch( SocketErr& e )
			{
				Log::instance().Trace( 50, "CScheduler::ListenThread: œô¨¸œô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ socket: %s", e.what() );
				continue;
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CScheduler::ListenThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CScheduler::ListenThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ" );
	}
	Log::instance().Trace( 50, "CScheduler::ListenThread: œô¨·œô¨Ðœô¨Òœô¨Õœô¨àœô¨èœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Ð œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Øœô¨å œô¨áœô¨Þœô¨Þœô¨Ñœô¨éœô¨Õœô¨Ýœô¨Øœô¨Ù" );
}

