#include "precomp.h"
#include "MessageParser.h"
#include "CTask.h"
#include "ping.h"

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
  Log::instance().Trace( 90, "%s: Message sended: %s", __FUNCTION__, Msg.ToString().c_str() );
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
  try
	{
	  //TODO: 
	  m_pScanner( m_strAddr.c_str(), CStartScan::CScanThreadTask::StorageFunc, 0 );
	}
  catch( std::exception& e )
	{
	  Log::instance().Trace( 0," %s: %s", __FUNCTION__, e.what() );
	}
  catch(...)
	{
	  Log::instance().Trace( 0,"%s: Unknown exception", __FUNCTION__ );
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
		Log::instance().Trace( 0, "%s:Storage not found for  %s : %s", __FUNCTION__, strAddress, strProtocolName );
		return;
	  }
	//TODO: 
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
	Log::instance().Trace( 90, "%s: Message sended: %s", __FUNCTION__, Msg.ToString().c_str() );
	return false;
  }

  void CStartScan::Execute( CEvent& CancelEv )
  {
	//TODO:
	Log::instance().Trace( 90, "%s", __FUNCTION__ );
	m_mtxCurState.Lock();
	m_CurState = SCANNING;
	m_mtxCurState.Unlock();

	m_mapStorages.clear();

	int iThreadsCount;
	Settings::instance().GetParam( SCAN_THREADS_COUNT, iThreadsCount );
	Log::instance().Trace( 10, "%s: Scan threads count: %d", __FUNCTION__, iThreadsCount );
	CThreadsPool pool( iThreadsCount );

	bool bPingOn;
	Settings::instance().GetParam( PING_ON, bPingOn );
	if( bPingOn )
	  {
		Log::instance().Trace( 10, "%s: Scan targets count: %d", __FUNCTION__, m_vecAddresses.size() );
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
			  Log::instance().Trace( 10, "%s: Target %s is not available, removing from scan list", __FUNCTION__, (*It)->GetAddress().c_str() );
		  }
		Log::instance().Trace( 10, "%s: Availability scan complete", __FUNCTION__ );
	  }

	bool bResolveOn;
    Settings::instance().GetParam( RESOLVE_HOST, bResolveOn );

	std::vector< std::string > vecHostNames;

	if( bResolveOn )
	  {
		Log::instance().Trace( 10, "%s: Starting names resolve", __FUNCTION__ );
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
			Log::instance().Trace( 5, "%s:Host name for address %s is %s", __FUNCTION__, m_vecAddresses[ std::distance( vecResolveTasks.begin(), It ) ].c_str(), (*It)->GetHostName().c_str() );
			vecHostNames.push_back( (*It)->GetHostName() );
		  }
		Log::instance().Trace( 10, "%s: Names resolve complete ", __FUNCTION__ );
	  }

	std::vector< SmartPtr< CScanThreadTask > > vecThreadTasks;
	Log::instance().Trace( 12, "%s: Hosts available for scan: %d", __FUNCTION__, m_vecAddresses.size() );

	for( std::vector< std::string >::iterator AddrIt = m_vecAddresses.begin(); AddrIt != m_vecAddresses.end(); AddrIt++ )
	  {
		for( PluginIterator PlugIt = m_PluginContainer.begin(); PlugIt != m_PluginContainer.end(); PlugIt++ )
		  {
		    if( CancelEv.TryWait() )
			  break;

			Log::instance().Trace( 80, "%s:Adding task for scan  %s with %s plugin", __FUNCTION__, AddrIt->c_str(), PlugIt->first.c_str() );

			vecThreadTasks.push_back( new CScanThreadTask( *AddrIt, PlugIt->second ) );

			m_mapStorages[ *AddrIt ][ PlugIt->first ] = SmartPtr< CTempStorage >( new CTempStorage( vecHostNames.empty()?"":vecHostNames[ std::distance( m_vecAddresses.begin(), AddrIt ) ], *AddrIt, std::string( PlugIt->first ) ) );

			pool.AddTask( vecThreadTasks.back() );

		  }
		if( CancelEv.TryWait() )
		  {
			Log::instance().Trace( 90, "%s: Scan canceled", __FUNCTION__ );
            //m_CancelEv.Reset();
			pool.CancelAllTasks();
			break;
		  }
	  }
	if( !pool.WaitAllComplete( CancelEv ) ){};
	//		pool.CancelAllTasks();

	Log::instance().Trace( 99, "%s: Scan finished", __FUNCTION__ );

	COutPacket Event;
	Event.PutField( EVENT_ID, SCAN_COMPLETE );
	Log::instance().Trace( 99, "%s: Sending event", __FUNCTION__ );
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
	Log::instance().Trace( 90, "%s: ", __FUNCTION__ );

	m_mtxCurState.Lock();

	if( SCANNING == m_CurState )
	  {
		Log::instance().Trace( 90, "%s: Scan process is being stoped", __FUNCTION__ );
		//TODO:StopScan
		//Cancel();
	  }
	else
	  Log::instance().Trace( 90, "%s: We are not in scanning state", __FUNCTION__ );

	m_mtxCurState.Unlock();

	COutPacket Msg;
	Msg.PutField( COMMAND_STAT, AGENT_RESP_OK );
	m_ServerHandler.SendMsg( Msg );
	Log::instance().Trace( 90, "%s: Message sended: %s", __FUNCTION__, Msg.ToString().c_str() );

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
	Log::instance().Trace( 90, "%s:", __FUNCTION__ );

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
			  }
			else
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

	Log::instance().Trace( 90, "%s: Message sended: %s", __FUNCTION__, Msg.ToString().c_str() );

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
