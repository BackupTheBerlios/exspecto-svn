#include "MessageParser.h"

std::map< int, CMessageParser::CreateTaskCallBack > CMessageParser::m_mapCreators;
//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CMessageParser------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
	
bool CMessageParser::RegisterCreator( int iTaskId, CreateTaskCallBack fnCreator )
{
	CMessageParser::m_mapCreators[ iTaskId ] = fnCreator;
	return true;
}
 
SmartPtr< CTask > CMessageParser::TaskFactory( BYTE bCommandId, CPacket& Msg )
{
/*	switch( bCommandId )
	{
		case GET_STATUS:
			return new CGetStatus( m_ServerHandler );
		case START_SCAN:
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
				vecAddresses.push_back( strAddress );
			}	
			return new CStartScan( m_ServerHandler, vecAddresses );
		}
		case GET_DATA:
			return new CGetData( m_ServerHandler );
		case STOP_SCAN:
			return new CStopScan( m_ServerHandler );
		default:
			return NULL;
	}*/
}

std::vector< SmartPtr< CTask > > CMessageParser::Parse( CPacket& Message )
{
	Log::instance().Trace( 95, "CMessageParser::Parse: Обработка входящего пакета" );
	BYTE bCommandId;
	std::vector< SmartPtr< CTask > > vecRes;
	SmartPtr< CTask > pTask;
	for(;;)
	{
		if( !Message.IsDone() )	
			Message.GetCommandId( bCommandId );
		else
		{
			Log::instance().Trace( 95, "CMessageParser::Parse: Обработка входящего пакета завершена.Всего команд:%d", vecRes.size() ); 
			break;
		}
		if( NULL != ( pTask = TaskFactory( bCommandId, Message ) ).get() )
			vecRes.push_back( pTask );
	}
	return vecRes;
}
