#include "precomp.h"
#include "MessageParser.h"
#include "commands.h"
#include "CTask.h"

enumAgentState CTask::m_CurState = Idling;
CCriticalSection CTask::m_csCurState;
CEvent CTask::m_CancelEv(false);
std::vector< std::string > CTask::m_vecData;
Container< CScanner*, PluginLoadStrategy > CStartScan::m_PluginContainer;

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
	m_bResult = Tools::Ping( m_strAddr, 3000, 1 );
	Log::instance().Trace( 10, "CStartScan::Execute: Ping %s END", m_strAddr.c_str() );
}


void CStartScan::CScanThreadTask::Execute( const CEvent& CancelEvent )
{
	m_pScanner->Scan( m_strAddr, m_vecData, CancelEvent );
}

CStartScan::CScanThreadTask::CScanThreadTask( const std::string& strAddr, CScanner* pScanner ):m_strAddr( strAddr )
																				  ,m_pScanner( pScanner )	
{
}

void CStartScan::CScanThreadTask::GetResData( std::vector< std::string >& vecResult )
{
	vecResult.insert( vecResult.end(), m_vecData.begin(), m_vecData.end() );
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
	m_vecData.clear();
	Log::instance().Trace( 90, "CStartScan: Поступил запрос на начало сканирования" );
	m_csCurState.Enter();
		m_CurState = Scanning;	
	m_csCurState.Leave();
	
	CThreadsPool pool( 50 );
	//Проверяем доступность хостов
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
	m_vecData.clear();
	for( std::vector< SmartPtr< CScanThreadTask > >::iterator It = vecThreadTasks.begin(); It != vecThreadTasks.end(); It++ )
		(*It)->GetResData( m_vecData );
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
	for( std::vector< std::string >::iterator It = m_vecData.begin(); It != m_vecData.end(); It++ )
	{
		//+1 - на завершающий ноль
		iSize += (int)It->size() + 1;
	}
	Log::instance().Trace( 90, "CGetData: Размер данных: %d", iSize );
	//4 байта на размер 1 байт - результат обработки команды
	iSize += 5;
	std::auto_ptr< BYTE > pbBuf = std::auto_ptr< BYTE >( new BYTE[ iSize ] );
	pbBuf.get()[0] = (BYTE)RESP_OK;
	::memcpy( pbBuf.get() + 1, (void*)&iSize, 4 );
	int iOffset = 5; 
	for( std::vector< std::string >::iterator It = m_vecData.begin(); It != m_vecData.end(); It++ )
	{
		strcpy( (char*)pbBuf.get() + iOffset, It->c_str() );
		iOffset += (int)It->size() + 1;
	}
	CPacket Msg;
	Msg.AddParam( pbBuf.get(), iSize );
	Msg.EndCommand();
	m_ServerHandler.SendMsg( Msg );
	//Log::instance().Dump( 90, pbBuf.get(), iSize, "CGetData:Immidiate: Отправлен ответ:" );
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
