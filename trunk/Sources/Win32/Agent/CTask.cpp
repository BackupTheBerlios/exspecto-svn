#include "precomp.h"
#include "MessageParser.h"
#include "commands.h"
#include "CTask.h"

//Максимальный размер пакета с данными для посылки 20MB
#define MAX_PACKET_SIZE 20971520

enumAgentState CTask::m_CurState = Idling;
CCriticalSection CTask::m_csCurState;
CEvent CTask::m_CancelEv(false);
std::map< std::string, filesStr > CTask::m_Data;
Container< CScanner*, PluginLoadStrategy > CStartScan::m_PluginContainer;
CTempStorage CTask::m_DataStorage( "temp.h" );

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CGetStatus----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

bool CGetStatus::Immidiate()
{
	m_csCurState.Enter();
		BYTE bResp[] = { RESP_OK, m_CurState };
		CPacket Msg;
		Msg.AddParam( bResp, sizeof(bResp) );
		m_ServerHandler.SendMsg( Msg );
		Log::instance().Dump( 90, bResp, sizeof( bResp ), "CGetStatus:Immidiate: Отправлен ответ:" );
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


void CStartScan::CScanThreadTask::Execute( const CEvent& CancelEvent )
{
	m_pScanner->Scan( m_strAddr, m_TaskData, CancelEvent );
	if( !m_TaskData.empty() )
		m_DataStorage.Put( m_strAddr );
	for( std::list<fileStr>::iterator It = m_TaskData.begin(); It != m_TaskData.end(); It++ )
	{
		//TODO: добавить синхронизацию
		m_DataStorage.Put( It->FileName );
		m_DataStorage.Put( It->FileSize );
		m_DataStorage.Put( It->FDate );
	}
	if( !m_TaskData.empty() )
	{
		static const int iHostEndMark = 0x1010;
		m_DataStorage.Put( iHostEndMark );
	}
}

CStartScan::CScanThreadTask::CScanThreadTask( const std::string& strAddr, CScanner* pScanner ):m_strAddr( strAddr )
																				  ,m_pScanner( pScanner )
																				  ,m_DataStorage( pScanner->GetProtocolName() + strAddr )
{
}

void CStartScan::CScanThreadTask::GetResData( std::map< std::string, filesStr >& Result )
{
	if( !m_TaskData.empty() )
		Result[ m_strAddr ].insert( Result[ m_strAddr ].end(), m_TaskData.begin(), m_TaskData.end() );
}

void CStartScan::CScanThreadTask::GetResData( CTempStorage& ResultStorage )
{
	ResultStorage << m_DataStorage;
}


void CStartScan::Load( CPacket& Msg )
{
	DWORD dwCount;
	std::string strAddress;
	std::vector< std::string > vecAddresses;
	//Получаем кол-во адресов в пакете
	Msg.GetParam( dwCount );
	for( unsigned int i = 0; i < dwCount; i++ )
	{
		//получаем очередной адрес
		Msg.GetAddress( strAddress );
		m_vecAddresses.push_back( strAddress );
		m_strDescription += strAddress;
		m_strDescription += " ";
	}	
}	
	
bool CStartScan::Immidiate()
{
	BYTE bResp[] = { RESP_OK };
	CPacket Msg;
	Msg.AddParam( bResp, sizeof(bResp) );
	m_ServerHandler.SendMsg( Msg );
	Log::instance().Dump( 90, bResp, sizeof( bResp ), "CStartScan:Immidiate: Отправлен ответ:" );
	return false;
}

void CStartScan::Execute()
{
	//TODO:проверять состояние перед началом сканирования
	m_Data.clear();
	Log::instance().Trace( 90, "CStartScan: Поступил запрос на начало сканирования" );
	m_csCurState.Enter();
		m_CurState = Scanning;	
	m_csCurState.Leave();

	int iThreadsCount;
	Settings::instance().GetParam( SCAN_THREADS_COUNT, iThreadsCount );
	Log::instance().Trace( 10, "CStartScan::Execute: Инициализируем пул потоков, кол-во потоков: %d", iThreadsCount );
	CThreadsPool pool( iThreadsCount );

	//Проверяем доступность хостов
	bool bPingOn;
	Settings::instance().GetParam( PING_ON, bPingOn );
	if( bPingOn )
	{
		Log::instance().Trace( 10, "CStartScan::Execute: Проверяем доступность хостов" );
		std::vector< SmartPtr< CAvailabilityScanTask > > vecAvailTasks;
		for( std::vector< std::string >::iterator It = m_vecAddresses.begin(); It != m_vecAddresses.end(); It++ )
		{
			vecAvailTasks.push_back( SmartPtr<CAvailabilityScanTask>( new CAvailabilityScanTask( *It ) ) );
			pool.AddTask( vecAvailTasks.back() );
		}
		Log::instance().Trace( 10, "CStartScan::Execute: WAITING" );
		if( !pool.WaitAllComplete( m_CancelEv ) )
			return;
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
	std::vector< SmartPtr< CScanThreadTask > > vecThreadTasks;

	for( std::vector< std::string >::iterator AddrIt = m_vecAddresses.begin(); AddrIt != m_vecAddresses.end(); AddrIt++ )
	{
		for( PluginIterator PlugIt = m_PluginContainer.begin(); PlugIt != m_PluginContainer.end(); PlugIt++ )
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( m_CancelEv, 0 ) )
				break;
			Log::instance().Trace( 80, "CStartScan: Добавляем задачу сканирвания адреса %s с помощью плагина %s", AddrIt->c_str(), (*PlugIt)->GetProtocolName() );
            vecThreadTasks.push_back( new CScanThreadTask( *AddrIt, *PlugIt ) );
			pool.AddTask( vecThreadTasks.back() );
		}
		if( WAIT_OBJECT_0 == WaitForSingleObject( m_CancelEv, 0 ) )
		{
			Log::instance().Trace( 90, "CStartScan: Сканирование отменено" );
			//Сбрасываем событие отмены
            m_CancelEv.Reset();
			pool.CancelAllTasks();
			break;
		}
	}
	pool.WaitAllComplete( m_CancelEv );
	Log::instance().Trace( 99, "CStartScan::Execute: Сканирование закончено" );
	m_Data.clear();
	for( std::vector< SmartPtr< CScanThreadTask > >::iterator It = vecThreadTasks.begin(); It != vecThreadTasks.end(); It++ )
		(*It)->GetResData( m_DataStorage );
	CPacket Event;
	BYTE bEvent = ScanComplete;
	Event.AddParam( &bEvent, 1 );
	Log::instance().Trace( 99, "CStartScan::Execute: Отправляем событие окончания сканирования" );
	m_ServerHandler.SendEvent( Event );
	m_csCurState.Enter();
		m_CurState = Idling;		
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
		if( Scanning == m_CurState )
		{
			Log::instance().Trace( 90, "CStopScan: Отменяем текущее сканирование" );
			Cancel();
		}else
			Log::instance().Trace( 90, "CStopScan: В данный момент не находится в состоянии сканирования" );
	m_csCurState.Leave();
	BYTE bResp[] = { RESP_OK };	
	CPacket Msg;
	Msg.AddParam( bResp, sizeof(bResp) );
	m_ServerHandler.SendMsg( Msg );
	Log::instance().Dump( 90, bResp, sizeof( bResp ), "CStopScan:Immidiate: Отправлен ответ:" );
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

bool CGetData::Immidiate()
{
	Log::instance().Trace( 90, "CGetData: Поступил запрос на получение данных" );
	//Подсчитываем размер данных для отправки
	unsigned int iSize = 0;
	static const BYTE pbEnd[] = { 0,0,0,0 };
/*	
	for( std::map< std::string, filesStr >::iterator ItData = m_Data.begin(); ItData != m_Data.end(); ItData++ )
	{
		iSize += (int)ItData->first.size() + 1;
		for( std::list< fileStr >::iterator It = ItData->second.begin(); It != ItData->second.end(); It++ )
		{
			//+1 - на завершающий ноль
			iSize += (int)It->FileName.size() + 1;
			iSize += sizeof( __int64 ) + sizeof( fileDate );
		}
		iSize += sizeof( pbEnd );
	}
	Log::instance().Trace( 90, "CGetData: Размер данных: %d", iSize );
	//4 байта на размер 1 байт - результат обработки команды
	iSize += 5;
	std::auto_ptr< BYTE > pbBuf = std::auto_ptr< BYTE >( new BYTE[ iSize ] );
	
	pbBuf.get()[0] = (BYTE)RESP_OK;
	::memcpy( pbBuf.get() + 1, (void*)&iSize, 4 );
	int iOffset = 5; 
	for( std::map< std::string, filesStr >::iterator ItData = m_Data.begin(); ItData != m_Data.end(); ItData++ )
	{
		strcpy( (char*)pbBuf.get() + iOffset, ItData->first.c_str() );
		iOffset += (int)ItData->first.size()+1;
		for( std::list< fileStr >::iterator It = ItData->second.begin(); It != ItData->second.end(); )
		{
			strcpy( (char*)pbBuf.get() + iOffset, It->FileName.c_str() );
			iOffset += (int)It->FileName.size()+1;
			memcpy( (char*)pbBuf.get() + iOffset, (void*)&It->FileSize, sizeof( __int64 ) );
			iOffset += sizeof( __int64 );
			memcpy( (char*)pbBuf.get() + iOffset, (void*)&It->FDate, sizeof( fileDate ) );
			iOffset += sizeof( fileDate );
			std::list< fileStr >::iterator ItTmp = It;
			It++;
			//Очищаем память,чтобы не было удваивания занимаемой памяти
			ItData->second.erase( ItTmp );
		}
		memcpy( (char*)pbBuf.get() + iOffset, pbEnd, sizeof( pbEnd ) );
		iOffset += sizeof( pbEnd );
	}
	m_Data.clear();
	*/

    CPacket Msg;
	unsigned long ulSize = m_DataStorage.Size();
	SmartPtr< BYTE > pbBuf = SmartPtr< BYTE >( new BYTE[sizeof( unsigned long ) + 1] );
	pbBuf.get()[0] = (BYTE)RESP_OK;
	::memcpy( pbBuf.get() + 1, (void*)&ulSize, sizeof( unsigned long ) );
	Msg.SetBuffer( pbBuf.get(), sizeof( unsigned long ) + 1 );
	if( 0 == ulSize )
		m_ServerHandler.SendMsg( Msg );
	else
		m_ServerHandler.SendMsg( Msg, false );
	unsigned long ulCount = MAX_PACKET_SIZE;
	int j = ulSize%MAX_PACKET_SIZE;
	for( unsigned long i = 0; i < ulSize; i += MAX_PACKET_SIZE )
	{
		pbBuf = m_DataStorage.GetBuf( ulCount );
		Msg.SetBuffer( pbBuf.get(), ulCount );
		m_ServerHandler.SendMsg( Msg );
	}
	m_DataStorage.Clear();
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
