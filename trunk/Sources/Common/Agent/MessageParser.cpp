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
		Log::instance().Trace( 95, "CMessageParser::GetRegisterCreator: œô¨·œô¨Ðœô¨àœô¨Õœô¨Óœô¨Øœô¨áœô¨âœô¨àœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ý œô¨âœô¨Øœô¨ß œô¨áœô¨Þœô¨Þœô¨Ñœô¨éœô¨Õœô¨Ýœô¨Øœô¨Ù %s", strTaskId.c_str() );
		m_mapCreators[ strTaskId ] = fnCreator;
	}else if( m_mapCreators.find( strTaskId ) != m_mapCreators.end() )
	{
		Log::instance().Trace( 95, "CMessageParser::GetRegisterCreator: œô¨·œô¨Ðœô¨ßœô¨àœô¨Þœô¨èœô¨Õœô¨Ý œô¨âœô¨Øœô¨ß œô¨áœô¨Þœô¨Þœô¨Ñœô¨éœô¨Õœô¨Ýœô¨Øœô¨Ù %s", strTaskId.c_str() );
		return m_mapCreators[ strTaskId ];
	}
	return NULL;
}

SmartPtr< CTask > CMessageParser::Parse( CInPacket& Message )
{
	Log::instance().Trace( 95, "CMessageParser::Parse: œô¨¾œô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨Úœô¨Ð œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Õœô¨Óœô¨Þ œô¨ßœô¨Ðœô¨Úœô¨Õœô¨âœô¨Ð" );
	SmartPtr< CTask > pTask;
	std::string strCommandId;
	Message.GetField( COMMAND_ID, strCommandId );
	CreateTaskCallBack fnCreator;
	if( ( fnCreator = GetRegisterCreator( strCommandId ) ) != NULL )
	{
		pTask = fnCreator( m_ServerHandler );
		pTask->Load( Message );
	}else
		Log::instance().Trace( 95, "CMessageParser::Parse: œô¨½œô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨ëœô¨Ù œô¨âœô¨Øœô¨ß œô¨áœô¨Þœô¨Þœô¨Ñœô¨éœô¨Õœô¨Ýœô¨Øœô¨ï: %s", strCommandId.c_str() );
	return pTask;
}
