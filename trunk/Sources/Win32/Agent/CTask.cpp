#include "MessageParser.h"

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
	CMessageParser::CreateTaskCallBack GSReged = CMessageParser::GetRegisterCreator( GET_STATUS, GetStatusCreator );
};

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CStartScan----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
	
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

	for( std::vector< std::string >::iterator AddrIt = m_vecAddresses.begin(); AddrIt != m_vecAddresses.end(); AddrIt++ )
	{
		for( PluginIterator PlugIt = m_PluginContainer.begin(); PlugIt != m_PluginContainer.end(); PlugIt++ )
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( m_CancelEv, 0 ) )
				break;
			Log::instance().Trace( 80, "CStartScan: Сканируем адрес %s с помощью плагина %s", AddrIt->c_str(), (*PlugIt)->GetProtocolName() );
			(*PlugIt)->Scan( *AddrIt, m_vecData, m_CancelEv );
		}
		if( WAIT_OBJECT_0 == WaitForSingleObject( m_CancelEv, 0 ) )
		{
			Log::instance().Trace( 90, "CStartScan: Сканирование отменено" );
			//Сбрасываем событие отмены
			m_CancelEv.Reset();
			break;
		}
	}
	//TODO:Уточнить,возможно следует использовать другой механизм блокировки,либо спин блокировку 
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
	int iSize = 0;
	for( std::vector< std::string >::iterator It = m_vecData.begin(); It != m_vecData.end(); It++ )
	{
		//+1 - на завершающий ноль
		iSize += It->size() + 1;
	}
	//4 байта на размер 1 байт - результат обработки команды
	iSize += 4 + 1; 
	std::auto_ptr< BYTE > pbBuf = std::auto_ptr< BYTE >( new BYTE[ iSize ] );
	pbBuf.get()[0] = (BYTE)RESP_OK;
	::memcpy( pbBuf .get() + 1, (void*)&iSize, 4 );
	int iOffset = 5; 
	for( std::vector< std::string >::iterator It = m_vecData.begin(); It != m_vecData.end(); It++ )
	{
		strcpy( (char*)pbBuf.get() + iOffset, It->c_str() );
		iOffset += It->size() + 1;
	}
	CPacket Msg;
	Msg.AddParam( pbBuf.get(), iSize );
	m_ServerHandler.SendMsg( Msg );
	Log::instance().Dump( 90, pbBuf.get(), iSize, "CGetData:Immidiate: Отправлен ответ:" );
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
