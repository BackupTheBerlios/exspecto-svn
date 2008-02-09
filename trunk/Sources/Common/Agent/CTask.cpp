#include "precomp.h"
#include "MessageParser.h"
#include "CTask.h"
#include "ping.h"

//�������М��ڜ�����؜��ܜ��М��ۜ�����ݜ������ �������М��ל��ܜ��՜��� ���ߜ��М��ڜ��՜������ ���� ���Ԝ��М��ݜ��ݜ�����ܜ��� ���Ԝ��ۜ��� ���ߜ��ޜ��������ۜ��ڜ��� 2MB
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
	Log::instance().Trace( 90, "CGetStatus:Immidiate: ����������ߜ������М��Ҝ��ۜ��՜��� ���ޜ�����Ҝ��՜���: %s", Msg.ToString().c_str() );
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
	    //TODO: �������М��ל��ޜ��ќ������М������������ ���� ���ޜ�����ܜ��՜��ݜ��ޜ���
		m_pScanner( m_strAddr.c_str(), CStartScan::CScanThreadTask::StorageFunc, 0 );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 0, "CStartScan::CScanThreadTask::Execute: ����������ڜ��ۜ��������՜��ݜ��؜���: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 0, "CStartScan::CScanThreadTask::Execute: �������՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ������ڜ��ۜ��������՜��ݜ��؜���" );
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
		Log::instance().Trace( 0, "CStartScan::CScanThreadTask::StorageFunc: �������� ���ݜ��М��ٜ��Ԝ��՜��ݜ��� ����������М��ݜ��؜��ۜ��؜������ ���Ԝ��ۜ��� ���Ԝ��М��ݜ��ݜ������, ���М��Ԝ������՜���: %s, ���ߜ������ޜ�����ޜ��ڜ��ޜ���: %s", strAddress, strProtocolName );
		return;
	}
	//TODO: ���ߜ��՜������՜��Ԝ��՜��ۜ��М������ ���؜��ݜ�����՜���������՜��ٜ��� StorageFunc, ���������ޜ��ќ��� ���ޜ��� ���؜�����ߜ��ޜ��ۜ�����ל��ޜ��Ҝ��М��� FileStr
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
	Log::instance().Trace( 90, "CStartScan:Immidiate: ����������ߜ������М��Ҝ��ۜ��՜��� ���ޜ�����Ҝ��՜���" );
	return false;
}

void CStartScan::Execute( CEvent& CancelEv )
{
	//TODO:���ߜ������ޜ��Ҝ��՜������������� ������ޜ��������ޜ�����ݜ��؜��� ���ߜ��՜������՜��� ���ݜ��М�����М��ۜ��ޜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���
	Log::instance().Trace( 90, "CStartScan: �������ޜ�����������ߜ��؜��� ���ל��М��ߜ������ޜ��� ���ݜ��� ���ݜ��М�����М��ۜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���" );
	m_mtxCurState.Lock();
		m_CurState = SCANNING;
	m_mtxCurState.Unlock();

	m_mapStorages.clear();

	int iThreadsCount;
	Settings::instance().GetParam( SCAN_THREADS_COUNT, iThreadsCount );
	Log::instance().Trace( 10, "CStartScan::Execute: �������ݜ��؜�����؜��М��ۜ��؜��ל��؜���������՜��� ���ߜ������ ���ߜ��ޜ�����ޜ��ڜ��ޜ���, ���ڜ��ޜ���-���Ҝ��� ���ߜ��ޜ�����ޜ��ڜ��ޜ���: %d", iThreadsCount );
	CThreadsPool pool( iThreadsCount );

	//�����������ޜ��Ҝ��՜���������՜��� ���Ԝ��ޜ�����������ߜ��ݜ��ޜ��������� ������ޜ��������ޜ���
	bool bPingOn;
	Settings::instance().GetParam( PING_ON, bPingOn );
	if( bPingOn )
	{
		Log::instance().Trace( 10, "CStartScan::Execute: �����������ޜ��Ҝ��՜���������՜��� ���Ԝ��ޜ�����������ߜ��ݜ��ޜ��������� ������ޜ��������ޜ���, ���ڜ��ޜ���-���Ҝ��� ������ޜ��������ޜ���: %d", m_vecAddresses.size() );
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
				Log::instance().Trace( 10, "CScheduler::OnStartScan: ���Ŝ��ޜ������ %s ���ݜ��� ���Ԝ��ޜ�����������ߜ��՜���. ����������ڜ��ۜ��������М��՜��� ���؜��� ������ߜ��؜�����ڜ��� ������՜��ڜ��������՜��Ӝ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���", (*It)->GetAddress().c_str() );
		}
		Log::instance().Trace( 10, "CStartScan::Execute: �����������ޜ��Ҝ��՜������ڜ��� ���Ԝ��ޜ�����������ߜ��ݜ��ޜ��������� ���ל��М��ڜ��ޜ��ݜ�����՜��ݜ���" );
	}
	//�������ޜ��ۜ��������М��՜��� ���؜��ܜ��՜��ݜ��� ������ޜ��������ޜ��� ���ߜ��� ���М��Ԝ������՜�����М���
	bool bResolveOn;
    Settings::instance().GetParam( RESOLVE_HOST, bResolveOn );
	std::vector< std::string > vecHostNames;
	if( bResolveOn )
	{
		Log::instance().Trace( 10, "%s: �������ޜ��ۜ��������М��՜��� ���؜��ܜ��՜��ݜ��� ������ޜ��������ޜ���", __FUNCTION__ );
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
			Log::instance().Trace( 5, "%s: �������ܜ��� ������ޜ��������� ���� ���М��Ԝ������՜�����ޜ��� %s: %s", __FUNCTION__, m_vecAddresses[ std::distance( vecResolveTasks.begin(), It ) ].c_str(), (*It)->GetHostName().c_str() );
			vecHostNames.push_back( (*It)->GetHostName() );
		}
		Log::instance().Trace( 10, "CStartScan::Execute: �������ޜ��ۜ��������՜��ݜ��؜��� ���؜��ܜ��՜��� ������ޜ��������ޜ��� ���ޜ��ڜ��ޜ��ݜ�����՜��ݜ���" );
	}

	std::vector< SmartPtr< CScanThreadTask > > vecThreadTasks;
	Log::instance().Trace( 12, "CStartScan::Execute: ����������՜��Ӝ��� ���М��Ԝ������՜�����ޜ��� ���Ԝ��ۜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���: %d", m_vecAddresses.size() );

	for( std::vector< std::string >::iterator AddrIt = m_vecAddresses.begin(); AddrIt != m_vecAddresses.end(); AddrIt++ )
	{
		for( PluginIterator PlugIt = m_PluginContainer.begin(); PlugIt != m_PluginContainer.end(); PlugIt++ )
		{
		    if( CancelEv.TryWait() )
                break;

			Log::instance().Trace( 80, "CStartScan: �������ޜ��ќ��М��Ҝ��ۜ�����՜��� ���ל��М��Ԝ��М������ ������ڜ��М��ݜ��؜������Ҝ��М��ݜ��؜��� ���М��Ԝ������՜������ %s ���� ���ߜ��ޜ��ܜ��ޜ��������� ���ߜ��ۜ��М��Ӝ��؜��ݜ��� %s", AddrIt->c_str(), PlugIt->first.c_str() );
			vecThreadTasks.push_back( new CScanThreadTask( *AddrIt, PlugIt->second ) );
			//�������ޜ��ל��Ԝ��М��՜��� ���Ҝ������՜��ܜ��՜��ݜ��ݜ��ޜ��� ����������М��ݜ��؜��ۜ��؜������ ���Ԝ��ۜ��� ���Ԝ��М��ݜ��ݜ������ ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���
			m_mapStorages[ *AddrIt ][ PlugIt->first ] = SmartPtr< CTempStorage >( new CTempStorage( vecHostNames.empty()?"":vecHostNames[ std::distance( m_vecAddresses.begin(), AddrIt ) ], *AddrIt, std::string( PlugIt->first ) ) );
			pool.AddTask( vecThreadTasks.back() );

		}
		if( CancelEv.TryWait() )
		{
			Log::instance().Trace( 90, "CStartScan: �������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ޜ�����ܜ��՜��ݜ��՜��ݜ���" );
			//�������ќ������М��������Ҝ��М��՜��� ������ޜ��ќ��������؜��� ���ޜ�����ܜ��՜��ݜ���
            //m_CancelEv.Reset();
			pool.CancelAllTasks();
			break;
		}
	}
	if( !pool.WaitAllComplete( CancelEv ) ){};
//		pool.CancelAllTasks();
	Log::instance().Trace( 99, "CStartScan::Execute: �������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ל��М��ڜ��ޜ��ݜ�����՜��ݜ���" );
	COutPacket Event;
	Event.PutField( EVENT_ID, SCAN_COMPLETE );
	Log::instance().Trace( 99, "CStartScan::Execute: ����������ߜ������М��Ҝ��ۜ�����՜��� ������ޜ��ќ��������؜��� ���ޜ��ڜ��ޜ��ݜ�����М��ݜ��؜��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���" );
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
	Log::instance().Trace( 90, "CStopScan: �������ޜ�����������ߜ��؜��� ���ל��М��ߜ������ޜ��� ���ݜ��� ���ޜ�����ܜ��՜��ݜ��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���" );
	m_mtxCurState.Lock();
		if( SCANNING == m_CurState )
		{
			Log::instance().Trace( 90, "CStopScan: ����������ܜ��՜��ݜ�����՜��� ������՜��ڜ��������՜��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���" );
			//TODO:StopScan ���ݜ��� �������М��ќ��ޜ�����М��՜���
			//Cancel();
		}else
			Log::instance().Trace( 90, "CStopScan: ���� ���Ԝ��М��ݜ��ݜ������ ���ܜ��ޜ��ܜ��՜��ݜ��� ���ݜ��� ���ݜ��М�����ޜ��Ԝ��؜��������� ���� ������ޜ��������ޜ�����ݜ��؜��� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���" );
	m_mtxCurState.Unlock();
	COutPacket Msg;
	Msg.PutField( COMMAND_STAT, AGENT_RESP_OK );
	m_ServerHandler.SendMsg( Msg );
	Log::instance().Trace( 90, "CStopScan:Immidiate: ����������ߜ������М��Ҝ��ۜ��՜��� ���ޜ�����Ҝ��՜���: %s", Msg.ToString().c_str() );
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
	Log::instance().Trace( 90, "CGetData: �������ޜ�����������ߜ��؜��� ���ל��М��ߜ������ޜ��� ���ݜ��� ���ߜ��ޜ��ۜ��������՜��ݜ��؜��� ���Ԝ��М��ݜ��ݜ������" );

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
	Log::instance().Trace( 90, "CGetData: �������М��ݜ��ݜ������ ���ޜ�����ߜ������М��Ҝ��ۜ��՜��ݜ���" );
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
