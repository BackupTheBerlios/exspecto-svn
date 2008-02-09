#include "precomp.h"
#include "MessageParser.h"
#include "CTask.h"
#include "ping.h"

//œô¨¼œô¨Ðœô¨Úœô¨áœô¨Øœô¨Üœô¨Ðœô¨Ûœô¨ìœô¨Ýœô¨ëœô¨Ù œô¨àœô¨Ðœô¨×œô¨Üœô¨Õœô¨à œô¨ßœô¨Ðœô¨Úœô¨Õœô¨âœô¨Ð œô¨á œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨Üœô¨Ø œô¨Ôœô¨Ûœô¨ï œô¨ßœô¨Þœô¨áœô¨ëœô¨Ûœô¨Úœô¨Ø 2MB
#define MAX_PACKET_SIZE  2097152

std::string CTask::m_CurState = IDLING;
CMutex CTask::m_mtxCurState;
//CEvent CTask::m_CancelEv(false);
PluginContainer CStartScan::m_PluginContainer;
CMutex CTask::m_mtxStorages;
std::map< std::string, std::map< std::string, SmartPtr<CTempStorage> > > CTask::m_mapStorages;

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CGetStatus----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

bool CGetStatus::Immidiate()
{
    CLock lock( m_mtxCurState );
	COutPacket Msg;
	Msg.PutField( COMMAND_STAT, AGENT_RESP_OK );
	Msg.PutField( AGENT_STATUS, m_CurState );
	m_ServerHandler.SendMsg( Msg );
	Log::instance().Trace( 90, "CGetStatus:Immidiate: œô¨¾œô¨âœô¨ßœô¨àœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ý œô¨Þœô¨âœô¨Òœô¨Õœô¨â: %s", Msg.ToString().c_str() );
	return true;
}
namespace
{
	CTask* GetStatusCreator( CServerHandler& Handler )
	{
		return new CGetStatus( Handler );
	}
	CMessageParser::CreateTaskCallBack GsScReged = CMessageParser::GetRegisterCreator( GET_STATUS, GetStatusCreator );
};

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CStartScan----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void CStartScan::CAvailabilityScanTask::Execute( CEvent& CancelEvent )
{
	Log::instance().Trace( 10, "CStartScan::Execute: Ping %s", m_strAddr.c_str() );
	m_bResult = Tools::PingHelper::instance().Ping( m_strAddr, 3000, 1 );
	Log::instance().Trace( 10, "CStartScan::Execute: Ping %s END", m_strAddr.c_str() );
}


void CStartScan::CResolveTask::Execute( CEvent& CancelEvent )
{
	Log::instance().Trace( 10, "CStartScan::Execute: Ping %s", m_strAddr.c_str() );
	hostent* res;
	unsigned long addr = inet_addr( m_strAddr.c_str() );
	if( NULL != ( res = ::gethostbyaddr( (char *) &addr, 4, AF_INET ) ) )
		m_strHostName = res->h_name;
}

void CStartScan::CScanThreadTask::Execute( CEvent& CancelEvent )
{
	try{
	    //TODO: œô¨àœô¨Ðœô¨×œô¨Þœô¨Ñœô¨àœô¨Ðœô¨âœô¨ìœô¨áœô¨ï œô¨á œô¨Þœô¨âœô¨Üœô¨Õœô¨Ýœô¨Þœô¨Ù
		m_pScanner( m_strAddr.c_str(), CStartScan::CScanThreadTask::StorageFunc, 0 );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 0, "CStartScan::CScanThreadTask::Execute: œô¨¸œô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 0, "CStartScan::CScanThreadTask::Execute: œô¨½œô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ" );
	}
}

void CStartScan::CScanThreadTask::StorageFunc( const char* strAddress
											 , const char* strProtocolName
											 , const char* strFileName
											 , long long FileSize
											 , int lFileTime
											 , int hFileTime )
{
	StoragesIt It;
	if( ( m_mapStorages.end() == ( It = m_mapStorages.find( strAddress ) ) )
		|| ( It->second.end() == It->second.find( strProtocolName ) ) )
	{
		Log::instance().Trace( 0, "CStartScan::CScanThreadTask::StorageFunc: œô¨½œô¨Õ œô¨Ýœô¨Ðœô¨Ùœô¨Ôœô¨Õœô¨Ýœô¨Þ œô¨åœô¨àœô¨Ðœô¨Ýœô¨Øœô¨Ûœô¨Øœô¨éœô¨Õ œô¨Ôœô¨Ûœô¨ï œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å, œô¨Ðœô¨Ôœô¨àœô¨Õœô¨á: %s, œô¨ßœô¨àœô¨Þœô¨âœô¨Þœô¨Úœô¨Þœô¨Û: %s", strAddress, strProtocolName );
		return;
	}
	//TODO: œô¨ßœô¨Õœô¨àœô¨Õœô¨Ôœô¨Õœô¨Ûœô¨Ðœô¨âœô¨ì œô¨Øœô¨Ýœô¨âœô¨Õœô¨àœô¨äœô¨Õœô¨Ùœô¨á StorageFunc, œô¨çœô¨âœô¨Þœô¨Ñœô¨ë œô¨Þœô¨Ý œô¨Øœô¨áœô¨ßœô¨Þœô¨Ûœô¨ìœô¨×œô¨Þœô¨Òœô¨Ðœô¨Û FileStr
	fileStr File;
	File.FileSize = FileSize;
	File.FileName = strFileName;
	ULARGE_INTEGER ulFileTime;
	ulFileTime.LowPart = lFileTime;
	ulFileTime.HighPart = hFileTime;
	File.FDate.UTS = (time_t)(( ulFileTime.QuadPart - 0x19DB1DED53E8000LL ) / 10000000);
	m_mapStorages[ strAddress ][ strProtocolName ]->PutRecord( File );
}

CStartScan::CScanThreadTask::CScanThreadTask( const std::string& strAddr, ScanFunc pScanner ):m_strAddr( strAddr )
																							,m_pScanner( pScanner )
{
}



void CStartScan::Load( CInPacket& Msg )
{
	std::string strAddress;
	Msg.GetFirstAddress( strAddress );
	do
	{
		m_vecAddresses.push_back( strAddress );
		m_strDescription += strAddress;
		m_strDescription += " ";
	}while( Msg.GetNextAddress( strAddress ) );
}

bool CStartScan::Immidiate()
{
	COutPacket Msg;
	Msg.PutField( COMMAND_STAT, AGENT_RESP_OK );
	m_ServerHandler.SendMsg( Msg );
	Log::instance().Trace( 90, "CStartScan:Immidiate: œô¨¾œô¨âœô¨ßœô¨àœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ý œô¨Þœô¨âœô¨Òœô¨Õœô¨â" );
	return false;
}

void CStartScan::Execute( CEvent& CancelEv )
{
	//TODO:œô¨ßœô¨àœô¨Þœô¨Òœô¨Õœô¨àœô¨ïœô¨âœô¨ì œô¨áœô¨Þœô¨áœô¨âœô¨Þœô¨ïœô¨Ýœô¨Øœô¨Õ œô¨ßœô¨Õœô¨àœô¨Õœô¨Ô œô¨Ýœô¨Ðœô¨çœô¨Ðœô¨Ûœô¨Þœô¨Ü œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï
	Log::instance().Trace( 90, "CStartScan: œô¨¿œô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Øœô¨Û œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á œô¨Ýœô¨Ð œô¨Ýœô¨Ðœô¨çœô¨Ðœô¨Ûœô¨Þ œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï" );
	m_mtxCurState.Lock();
		m_CurState = SCANNING;
	m_mtxCurState.Unlock();

	m_mapStorages.clear();

	int iThreadsCount;
	Settings::instance().GetParam( SCAN_THREADS_COUNT, iThreadsCount );
	Log::instance().Trace( 10, "CStartScan::Execute: œô¨¸œô¨Ýœô¨Øœô¨æœô¨Øœô¨Ðœô¨Ûœô¨Øœô¨×œô¨Øœô¨àœô¨ãœô¨Õœô¨Ü œô¨ßœô¨ãœô¨Û œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Þœô¨Ò, œô¨Úœô¨Þœô¨Û-œô¨Òœô¨Þ œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Þœô¨Ò: %d", iThreadsCount );
	CThreadsPool pool( iThreadsCount );

	//œô¨¿œô¨àœô¨Þœô¨Òœô¨Õœô¨àœô¨ïœô¨Õœô¨Ü œô¨Ôœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Ýœô¨Þœô¨áœô¨âœô¨ì œô¨åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò
	bool bPingOn;
	Settings::instance().GetParam( PING_ON, bPingOn );
	if( bPingOn )
	{
		Log::instance().Trace( 10, "CStartScan::Execute: œô¨¿œô¨àœô¨Þœô¨Òœô¨Õœô¨àœô¨ïœô¨Õœô¨Ü œô¨Ôœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Ýœô¨Þœô¨áœô¨âœô¨ì œô¨åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò, œô¨Úœô¨Þœô¨Û-œô¨Òœô¨Þ œô¨åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò: %d", m_vecAddresses.size() );
		std::vector< SmartPtr< CAvailabilityScanTask > > vecAvailTasks;
		for( std::vector< std::string >::iterator It = m_vecAddresses.begin(); It != m_vecAddresses.end(); It++ )
		{
			vecAvailTasks.push_back( SmartPtr<CAvailabilityScanTask>( new CAvailabilityScanTask( *It ) ) );
			pool.AddTask( vecAvailTasks.back() );
		}
		Log::instance().Trace( 10, "CStartScan::Execute: WAITING" );
		if( !pool.WaitAllComplete( CancelEv ) )
		{
			pool.CancelAllTasks();
			return;
		}
		m_vecAddresses.clear();
		for( std::vector< SmartPtr< CAvailabilityScanTask > >::iterator It = vecAvailTasks.begin(); It != vecAvailTasks.end(); It++ )
		{
			if( (*It)->IsAvailable() )
				m_vecAddresses.push_back( (*It)->GetAddress() );
			else
				Log::instance().Trace( 10, "CScheduler::OnStartScan: œô¨Åœô¨Þœô¨áœô¨â %s œô¨Ýœô¨Õ œô¨Ôœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Õœô¨Ý. œô¨¸œô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Ðœô¨Õœô¨Ü œô¨Øœô¨× œô¨áœô¨ßœô¨Øœô¨áœô¨Úœô¨Ð œô¨âœô¨Õœô¨Úœô¨ãœô¨éœô¨Õœô¨Óœô¨Þ œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï", (*It)->GetAddress().c_str() );
		}
		Log::instance().Trace( 10, "CStartScan::Execute: œô¨¿œô¨àœô¨Þœô¨Òœô¨Õœô¨àœô¨Úœô¨Ð œô¨Ôœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Ýœô¨Þœô¨áœô¨âœô¨Ø œô¨×œô¨Ðœô¨Úœô¨Þœô¨Ýœô¨çœô¨Õœô¨Ýœô¨Ð" );
	}
	//œô¨¿œô¨Þœô¨Ûœô¨ãœô¨çœô¨Ðœô¨Õœô¨Ü œô¨Øœô¨Üœô¨Õœô¨Ýœô¨Ð œô¨åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò œô¨ßœô¨Þ œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ðœô¨Ü
	bool bResolveOn;
    Settings::instance().GetParam( RESOLVE_HOST, bResolveOn );
	std::vector< std::string > vecHostNames;
	if( bResolveOn )
	{
		Log::instance().Trace( 10, "%s: œô¨¿œô¨Þœô¨Ûœô¨ãœô¨çœô¨Ðœô¨Õœô¨Ü œô¨Øœô¨Üœô¨Õœô¨Ýœô¨Ð œô¨åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò", __FUNCTION__ );
		std::vector< SmartPtr< CResolveTask > > vecResolveTasks;
		for( std::vector< std::string >::iterator It = m_vecAddresses.begin(); It != m_vecAddresses.end(); It++ )
		{
			vecResolveTasks.push_back( SmartPtr<CResolveTask>( new CResolveTask( *It ) ) );
			pool.AddTask( vecResolveTasks.back() );
		}
		if( !pool.WaitAllComplete( CancelEv ) )
		{
			pool.CancelAllTasks();
			return;
		}
		for( std::vector< SmartPtr< CResolveTask > >::iterator It = vecResolveTasks.begin(); It != vecResolveTasks.end(); It++ )
		{
			Log::instance().Trace( 5, "%s: œô¨¸œô¨Üœô¨ï œô¨åœô¨Þœô¨áœô¨âœô¨Ð œô¨á œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Þœô¨Ü %s: %s", __FUNCTION__, m_vecAddresses[ std::distance( vecResolveTasks.begin(), It ) ].c_str(), (*It)->GetHostName().c_str() );
			vecHostNames.push_back( (*It)->GetHostName() );
		}
		Log::instance().Trace( 10, "CStartScan::Execute: œô¨¿œô¨Þœô¨Ûœô¨ãœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Øœô¨Üœô¨Õœô¨Ý œô¨åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò œô¨Þœô¨Úœô¨Þœô¨Ýœô¨çœô¨Õœô¨Ýœô¨Þ" );
	}

	std::vector< SmartPtr< CScanThreadTask > > vecThreadTasks;
	Log::instance().Trace( 12, "CStartScan::Execute: œô¨²œô¨áœô¨Õœô¨Óœô¨Þ œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Þœô¨Ò œô¨Ôœô¨Ûœô¨ï œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï: %d", m_vecAddresses.size() );

	for( std::vector< std::string >::iterator AddrIt = m_vecAddresses.begin(); AddrIt != m_vecAddresses.end(); AddrIt++ )
	{
		for( PluginIterator PlugIt = m_PluginContainer.begin(); PlugIt != m_PluginContainer.end(); PlugIt++ )
		{
		    if( CancelEv.TryWait() )
                break;

			Log::instance().Trace( 80, "CStartScan: œô¨´œô¨Þœô¨Ñœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨Õœô¨Ü œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨çœô¨ã œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð %s œô¨á œô¨ßœô¨Þœô¨Üœô¨Þœô¨éœô¨ìœô¨î œô¨ßœô¨Ûœô¨Ðœô¨Óœô¨Øœô¨Ýœô¨Ð %s", AddrIt->c_str(), PlugIt->first.c_str() );
			vecThreadTasks.push_back( new CScanThreadTask( *AddrIt, PlugIt->second ) );
			//œô¨Áœô¨Þœô¨×œô¨Ôœô¨Ðœô¨Õœô¨Ü œô¨Òœô¨àœô¨Õœô¨Üœô¨Õœô¨Ýœô¨Ýœô¨Þœô¨Õ œô¨åœô¨àœô¨Ðœô¨Ýœô¨Øœô¨Ûœô¨Øœô¨éœô¨Õ œô¨Ôœô¨Ûœô¨ï œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï
			m_mapStorages[ *AddrIt ][ PlugIt->first ] = SmartPtr< CTempStorage >( new CTempStorage( vecHostNames.empty()?"":vecHostNames[ std::distance( m_vecAddresses.begin(), AddrIt ) ], *AddrIt, std::string( PlugIt->first ) ) );
			pool.AddTask( vecThreadTasks.back() );

		}
		if( CancelEv.TryWait() )
		{
			Log::instance().Trace( 90, "CStartScan: œô¨Áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨Þœô¨âœô¨Üœô¨Õœô¨Ýœô¨Õœô¨Ýœô¨Þ" );
			//œô¨Áœô¨Ñœô¨àœô¨Ðœô¨áœô¨ëœô¨Òœô¨Ðœô¨Õœô¨Ü œô¨áœô¨Þœô¨Ñœô¨ëœô¨âœô¨Øœô¨Õ œô¨Þœô¨âœô¨Üœô¨Õœô¨Ýœô¨ë
            //m_CancelEv.Reset();
			pool.CancelAllTasks();
			break;
		}
	}
	if( !pool.WaitAllComplete( CancelEv ) ){};
//		pool.CancelAllTasks();
	Log::instance().Trace( 99, "CStartScan::Execute: œô¨Áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨×œô¨Ðœô¨Úœô¨Þœô¨Ýœô¨çœô¨Õœô¨Ýœô¨Þ" );
	COutPacket Event;
	Event.PutField( EVENT_ID, SCAN_COMPLETE );
	Log::instance().Trace( 99, "CStartScan::Execute: œô¨¾œô¨âœô¨ßœô¨àœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨Õœô¨Ü œô¨áœô¨Þœô¨Ñœô¨ëœô¨âœô¨Øœô¨Õ œô¨Þœô¨Úœô¨Þœô¨Ýœô¨çœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï" );
	m_ServerHandler.SendEvent( Event );
	m_mtxCurState.Lock();
		m_CurState = IDLING;
	m_mtxCurState.Unlock();
}
namespace
{
	CTask* StartScanCreator( CServerHandler& Handler )
	{
		return new CStartScan( Handler );
	}
	CMessageParser::CreateTaskCallBack StScReged = CMessageParser::GetRegisterCreator( START_SCAN, StartScanCreator );
};
//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CStopScan-----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

bool CStopScan::Immidiate()
{
	Log::instance().Trace( 90, "CStopScan: œô¨¿œô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Øœô¨Û œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á œô¨Ýœô¨Ð œô¨Þœô¨âœô¨Üœô¨Õœô¨Ýœô¨ã œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï" );
	m_mtxCurState.Lock();
		if( SCANNING == m_CurState )
		{
			Log::instance().Trace( 90, "CStopScan: œô¨¾œô¨âœô¨Üœô¨Õœô¨Ýœô¨ïœô¨Õœô¨Ü œô¨âœô¨Õœô¨Úœô¨ãœô¨éœô¨Õœô¨Õ œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ" );
			//TODO:StopScan œô¨Ýœô¨Õ œô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨Ðœô¨Õœô¨â
			//Cancel();
		}else
			Log::instance().Trace( 90, "CStopScan: œô¨² œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨Ù œô¨Üœô¨Þœô¨Üœô¨Õœô¨Ýœô¨â œô¨Ýœô¨Õ œô¨Ýœô¨Ðœô¨åœô¨Þœô¨Ôœô¨Øœô¨âœô¨áœô¨ï œô¨Ò œô¨áœô¨Þœô¨áœô¨âœô¨Þœô¨ïœô¨Ýœô¨Øœô¨Ø œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï" );
	m_mtxCurState.Unlock();
	COutPacket Msg;
	Msg.PutField( COMMAND_STAT, AGENT_RESP_OK );
	m_ServerHandler.SendMsg( Msg );
	Log::instance().Trace( 90, "CStopScan:Immidiate: œô¨¾œô¨âœô¨ßœô¨àœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ý œô¨Þœô¨âœô¨Òœô¨Õœô¨â: %s", Msg.ToString().c_str() );
	return true;
}
namespace
{
	CTask* StopScanCreator( CServerHandler& Handler )
	{
		return new CStopScan( Handler );
	}
	CMessageParser::CreateTaskCallBack SSReged = CMessageParser::GetRegisterCreator( STOP_SCAN, StopScanCreator );
};
//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CGetData------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

void CGetData::Load( CInPacket& Msg )
{
	Msg.GetField( FILES_COUNT, m_iPacketSize );
}

bool CGetData::Immidiate()
{
	Log::instance().Trace( 90, "CGetData: œô¨¿œô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Øœô¨Û œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á œô¨Ýœô¨Ð œô¨ßœô¨Þœô¨Ûœô¨ãœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å" );

	COutPacket Msg;
	hostRec TmpHost;
	Msg.PutField( COMMAND_STAT, AGENT_RESP_OK );
	int iRecordsCount = 0;
	for( StoragesIt It = m_mapStorages.begin(); It != m_mapStorages.end() && iRecordsCount < m_iPacketSize;)
	{
		for( std::map< std::string, SmartPtr< CTempStorage > >::iterator ProtoIt = It->second.begin(); ProtoIt != It->second.end() && iRecordsCount < m_iPacketSize;)
		{
			iRecordsCount += ProtoIt->second->GetRecords( TmpHost, m_iPacketSize - iRecordsCount );
			Msg.PutHostRec( TmpHost, ProtoIt->first );
			if( ProtoIt->second->IsEof() )
			{
				ProtoIt->second->Clear();
				It->second.erase( ProtoIt++ );
			}else
				ProtoIt++;

		}
		if( It->second.empty() )
			m_mapStorages.erase( It++ );
		else
			It++;

	}
	Msg.PutField( FILES_COUNT, iRecordsCount );
	if( m_mapStorages.empty() )
		Msg.PutField( FILES_LEFT, "false" );
	else
		Msg.PutField( FILES_LEFT, "true" );
	m_ServerHandler.SendMsg( Msg );
	Log::instance().Trace( 90, "CGetData: œô¨´œô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨Õ œô¨Þœô¨âœô¨ßœô¨àœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨ë" );
	m_mapStorages.clear();
	return true;
}

namespace
{
	CTask* GetDataCreator( CServerHandler& Handler )
	{
		return new CGetData( Handler );
	}
	CMessageParser::CreateTaskCallBack GDReged = CMessageParser::GetRegisterCreator( GET_DATA, GetDataCreator );
};
