#include "precomp.h"
#include "MessageParser.h"
#include "CTask.h"

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CMessageParser------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
	
CMessageParser::CreateTaskCallBack CMessageParser::GetRegisterCreator( std::string strTaskId, CreateTaskCallBack fnCreator )
{
	static std::map< std::string, CreateTaskCallBack > m_mapCreators;
	if( NULL != fnCreator )
	{
		Log::instance().Trace( 95, "CMessageParser::GetRegisterCreator: Зарегистрирован тип сообщений %s", strTaskId.c_str() );
		m_mapCreators[ strTaskId ] = fnCreator;
	}else if( m_mapCreators.find( strTaskId ) != m_mapCreators.end() )
	{
		Log::instance().Trace( 95, "CMessageParser::GetRegisterCreator: Запрошен тип сообщений %s", strTaskId.c_str() );
		return m_mapCreators[ strTaskId ];
	}
	return NULL;
}

SmartPtr< CTask > CMessageParser::Parse( CInPacket& Message )
{
	Log::instance().Trace( 95, "CMessageParser::Parse: Обработка входящего пакета" );
	SmartPtr< CTask > pTask;
	std::string strCommandId;
	Message.GetField( COMMAND_ID, strCommandId );
	CreateTaskCallBack fnCreator;
	if( ( fnCreator = GetRegisterCreator( strCommandId ) ) != NULL )
	{
		pTask = fnCreator( m_ServerHandler );
		pTask->Load( Message );
	}else
		Log::instance().Trace( 95, "CMessageParser::Parse: Неизвестный тип сообщения: %s", strCommandId.c_str() );
	return pTask;
}
