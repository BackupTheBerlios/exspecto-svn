#include "precomp.h"
#include "MessageParser.h"
#include "commands.h"
#include "CTask.h"

//Максимальный размер пакета с данными для посылки 2MB
#define MAX_PACKET_SIZE  2097152

enumAgentState CTask::m_CurState = Idling;
CCriticalSection CTask::m_csCurState;
//CEvent CTask::m_CancelEv(false);
PluginContainer CStartScan::m_PluginContainer;
CTempStorage CTask::m_DataStorage( "temp.dat" );
CCriticalSection CTask::m_csStorages;
std::map< std::string, std::map< std::string, SmartPtr<CTempStorage> > > CTask::m_mapStorages;

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

	m_mapStorages[ strAddress ][ strProtocolName ]->Put( strAddress );
    m_mapStorages[ strAddress ][ strProtocolName ]->Put( strFileName );
    m_mapStorages[ strAddress ][ strProtocolName ]->Put( FileSize );
	m_mapStorages[ strAddress ][ strProtocolName ]->Put( lFileTime );
	m_mapStorages[ strAddress ][ strProtocolName ]->Put( hFileTime );
}

CStartScan::CScanThreadTask::CScanThreadTask( const std::string& strAddr, ScanFunc pScanner ):m_strAddr( strAddr )
																							,m_pScanner( pScanner )
{
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

void CStartScan::Execute( CEvent& CancelEv )
{
	//TODO:проверять состояние перед началом сканирования
	Log::instance().Trace( 90, "CStartScan: Поступил запрос на начало сканирования" );
	m_csCurState.Enter();
		m_CurState = Scanning;	
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
		Log::instance().Trace( 10, "CStartScan::Execute: Проверяем доступность хостов" );
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
			m_mapStorages[ *AddrIt ][ PlugIt->first ] = SmartPtr< CTempStorage >( new CTempStorage( std::string( PlugIt->first ) + *AddrIt ) );
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
			//Cancel();
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

	//Флаг, отмечающий первый пакет,в котором дополнительно должены быть отправлены размер данных
	//и код возврата
	bool bFirst = true;
	//считаем размер
	unsigned long ulSize = 0;
	for( StoragesIt It = m_mapStorages.begin(); It != m_mapStorages.end(); It++ )
		for( std::map< std::string, SmartPtr< CTempStorage > >::iterator ProtoIt = It->second.begin(); ProtoIt != It->second.end(); ProtoIt++ )
			ulSize += ProtoIt->second->Size();


    CPacket Msg;
	for( StoragesIt It = m_mapStorages.begin(); It != m_mapStorages.end(); It++ )
	{
		for( std::map< std::string, SmartPtr< CTempStorage > >::iterator ProtoIt = It->second.begin(); ProtoIt != It->second.end(); ProtoIt++ )
		{
			//Определяем последнее хранилище или нет,чтобы послать завершающий маркер пакета
			bool bLast = false;
			StoragesIt ItStoragesTmp = It;
			std::map< std::string, SmartPtr< CTempStorage > >::iterator ProtoTmpIt = ProtoIt;
			ItStoragesTmp++;ProtoTmpIt++;
			if( ( ItStoragesTmp == m_mapStorages.end() ) && ( ProtoTmpIt == It->second.end() ) )
				bLast = true;

			SmartPtr< BYTE, AllocNewArray<BYTE> > pbBuf;
			if( bFirst )
			{
				pbBuf = SmartPtr< BYTE, AllocNewArray<BYTE> >( new BYTE[sizeof( unsigned long ) + 1] );
				pbBuf.get()[0] = (BYTE)RESP_OK;
				::memcpy( pbBuf.get() + 1, (void*)&ulSize, sizeof( unsigned long ) );
				Msg.SetBuffer( pbBuf.get(), sizeof( unsigned long ) + 1 );
				if( ( 0 == ulSize ) && bLast )
					m_ServerHandler.SendMsg( Msg );
				else
					m_ServerHandler.SendMsg( Msg, false );
			}

			unsigned long ulCount = MAX_PACKET_SIZE;
			unsigned long ulFileSize = ProtoIt->second->Size();
			for( unsigned long i = 0; i < ulFileSize; i += MAX_PACKET_SIZE )
			{
				pbBuf = ProtoIt->second->GetBuf( ulCount );
				Msg.SetBuffer( pbBuf.get(), ulCount );
				m_ServerHandler.SendMsg( Msg, false );
			}
			//Отправляем маркер окончания пакета
			if( bLast )
			{
				pbBuf = SmartPtr< BYTE, AllocNewArray<BYTE> >( new BYTE[0] );
				Msg.SetBuffer( pbBuf.get(), 0 );
				m_ServerHandler.SendMsg( Msg );
			}
			//TODO: это похоже необязательное если оставить дальше m_mapStorages.clear();
			//ProtoIt->second->Clear();
			bFirst = false;
		}
	}
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
