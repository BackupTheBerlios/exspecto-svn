#include "precomp.h"
#include "MessageParser.h"
#include "CTask.h"

//Максимальный размер пакета с данными для посылки 2MB
#define MAX_PACKET_SIZE  2097152

std::string CTask::m_CurState = IDLING;
CCriticalSection CTask::m_csCurState;
//CEvent CTask::m_CancelEv(false);
PluginContainer CStartScan::m_PluginContainer;
CCriticalSection CTask::m_csStorages;
std::map< std::string, std::map< std::string, SmartPtr<CTempStorage> > > CTask::m_mapStorages;

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CGetStatus----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

bool CGetStatus::Immidiate()
{
	m_csCurState.Enter();
		COutPacket Msg;
		Msg.PutField( COMMAND_STAT, AGENT_RESP_OK );
		Msg.PutField( AGENT_STATUS, m_CurState );
		m_ServerHandler.SendMsg( Msg );
		Log::instance().Trace( 90, "CGetStatus:Immidiate: Отправлен ответ: %s", Msg.ToString().c_str() );
	m_csCurState.Leave();
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
void CStartScan::CAvailabilityScanTask::Execute( const CEvent& CancelEvent )
{
	Log::instance().Trace( 10, "CStartScan::Execute: Ping %s", m_strAddr.c_str() );
	m_bResult = Tools::PingHelper::instance().Ping( m_strAddr, 3000, 1 );
	Log::instance().Trace( 10, "CStartScan::Execute: Ping %s END", m_strAddr.c_str() );
}


void CStartScan::CResolveTask::Execute( const CEvent& CancelEvent )
{
	Log::instance().Trace( 10, "CStartScan::Execute: Ping %s", m_strAddr.c_str() );
	hostent* res;
	unsigned long addr = inet_addr( m_strAddr.c_str() );
	if( NULL != ( res = ::gethostbyaddr( (char *) &addr, 4, AF_INET ) ) )
		m_strHostName = res->h_name;
}

void CStartScan::CScanThreadTask::Execute( const CEvent& CancelEvent )
{
	try{
		m_pScanner( m_strAddr.c_str(), CStartScan::CScanThreadTask::StorageFunc, CancelEvent );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 0, "CStartScan::CScanThreadTask::Execute: Исключение: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 0, "CStartScan::CScanThreadTask::Execute: Неизвестное сключение" );
	}
}

void CStartScan::CScanThreadTask::StorageFunc( const char* strAddress
											 , const char* strProtocolName
											 , const char* strFileName
											 , __int64 FileSize
											 , DWORD lFileTime
											 , DWORD hFileTime )
{
	StoragesIt It;
	if( ( m_mapStorages.end() == ( It = m_mapStorages.find( strAddress ) ) )
		|| ( It->second.end() == It->second.find( strProtocolName ) ) )
	{
		Log::instance().Trace( 0, "CStartScan::CScanThreadTask::StorageFunc: Не найдено хранилище для данных, адрес: %s, протокол: %s", strAddress, strProtocolName );
		return;
	}
	//TODO: переделать интерфейс StorageFunc, чтобы он использовал FileStr
	fileStr File;
	File.FileSize = FileSize;
	File.FileName = strFileName;
	ULARGE_INTEGER ulFileTime;
	ulFileTime.LowPart = lFileTime;
	ulFileTime.HighPart = hFileTime;
	File.FDate.UTS = (time_t)(( ulFileTime.QuadPart - 0x19DB1DED53E8000 ) / 10000000);
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
	Log::instance().Trace( 90, "CStartScan:Immidiate: Отправлен ответ" );
	return false;
}

void CStartScan::Execute( CEvent& CancelEv )
{
	//TODO:проверять состояние перед началом сканирования
	Log::instance().Trace( 90, "CStartScan: Поступил запрос на начало сканирования" );
	m_csCurState.Enter();
		m_CurState = SCANNING;	
	m_csCurState.Leave();

	m_mapStorages.clear();

	int iThreadsCount;
	Settings::instance().GetParam( SCAN_THREADS_COUNT, iThreadsCount );
	Log::instance().Trace( 10, "CStartScan::Execute: Инициализируем пул потоков, кол-во потоков: %d", iThreadsCount );
	CThreadsPool pool( iThreadsCount );

	//Проверяем доступность хостов
	bool bPingOn;
	Settings::instance().GetParam( PING_ON, bPingOn );
	if( bPingOn )
	{
		Log::instance().Trace( 10, "CStartScan::Execute: Проверяем доступность хостов, кол-во хостов: %d", m_vecAddresses.size() );
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
				Log::instance().Trace( 10, "CScheduler::OnStartScan: Хост %s не доступен. Исключаем из списка текущего сканирования", (*It)->GetAddress().c_str() );
		}
		Log::instance().Trace( 10, "CStartScan::Execute: Проверка доступности закончена" );
	}
	//Получаем имена хостов по адресам
	bool bResolveOn;
    Settings::instance().GetParam( RESOLVE_HOST, bResolveOn );
	std::vector< std::string > vecHostNames;
	if( bResolveOn )
	{
		Log::instance().Trace( 10, "%s: Получаем имена хостов", __FUNCTION__ );
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
			Log::instance().Trace( 5, "%s: Имя хоста с адресом %s: %s", __FUNCTION__, m_vecAddresses[ std::distance( vecResolveTasks.begin(), It ) ].c_str(), (*It)->GetHostName().c_str() );
			vecHostNames.push_back( (*It)->GetHostName() );
		}
		Log::instance().Trace( 10, "CStartScan::Execute: Получение имен хостов окончено" );
	}

	std::vector< SmartPtr< CScanThreadTask > > vecThreadTasks;
	Log::instance().Trace( 12, "CStartScan::Execute: Всего адресов для сканирования: %d", m_vecAddresses.size() );

	for( std::vector< std::string >::iterator AddrIt = m_vecAddresses.begin(); AddrIt != m_vecAddresses.end(); AddrIt++ )
	{
		for( PluginIterator PlugIt = m_PluginContainer.begin(); PlugIt != m_PluginContainer.end(); PlugIt++ )
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( CancelEv, 0 ) )
				break;
			Log::instance().Trace( 80, "CStartScan: Добавляем задачу сканирвания адреса %s с помощью плагина %s", AddrIt->c_str(), PlugIt->first.c_str() );
			vecThreadTasks.push_back( new CScanThreadTask( *AddrIt, PlugIt->second ) );
			//Создаем временное хранилище для данных сканирования
			m_mapStorages[ *AddrIt ][ PlugIt->first ] = SmartPtr< CTempStorage >( new CTempStorage( vecHostNames.empty()?"":vecHostNames[ std::distance( m_vecAddresses.begin(), AddrIt ) ], *AddrIt, std::string( PlugIt->first ) ) );
			pool.AddTask( vecThreadTasks.back() );
			
		}
		if( WAIT_OBJECT_0 == WaitForSingleObject( CancelEv, 0 ) )
		{
			Log::instance().Trace( 90, "CStartScan: Сканирование отменено" );
			//Сбрасываем событие отмены
            //m_CancelEv.Reset();
			pool.CancelAllTasks();
			break;
		}
	}
	if( !pool.WaitAllComplete( CancelEv ) ){};
//		pool.CancelAllTasks();
	Log::instance().Trace( 99, "CStartScan::Execute: Сканирование закончено" );
	COutPacket Event;
	Event.PutField( EVENT_ID, SCAN_COMPLETE );
	Log::instance().Trace( 99, "CStartScan::Execute: Отправляем событие окончания сканирования" );
	m_ServerHandler.SendEvent( Event );
	m_csCurState.Enter();
		m_CurState = IDLING;
	m_csCurState.Leave();
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
	Log::instance().Trace( 90, "CStopScan: Поступил запрос на отмену сканирования" );
	m_csCurState.Enter();
		if( SCANNING == m_CurState )
		{
			Log::instance().Trace( 90, "CStopScan: Отменяем текущее сканирование" );
			//TODO:StopScan не работает
			//Cancel();
		}else
			Log::instance().Trace( 90, "CStopScan: В данный момент не находится в состоянии сканирования" );
	m_csCurState.Leave();
	COutPacket Msg;
	Msg.PutField( COMMAND_STAT, AGENT_RESP_OK );
	m_ServerHandler.SendMsg( Msg );
	Log::instance().Trace( 90, "CStopScan:Immidiate: Отправлен ответ: %s", Msg.ToString().c_str() );
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
	Log::instance().Trace( 90, "CGetData: Поступил запрос на получение данных" );
	
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
	Log::instance().Trace( 90, "CGetData: Данные отправлены" );
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
