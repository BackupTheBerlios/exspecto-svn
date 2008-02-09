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
		Log::instance().Trace( 95, "CMessageParser::GetRegisterCreator: �������М������՜��Ӝ��؜������������؜������ޜ��Ҝ��М��� ������؜��� ������ޜ��ޜ��ќ�����՜��ݜ��؜��� %s", strTaskId.c_str() );
		m_mapCreators[ strTaskId ] = fnCreator;
	}else if( m_mapCreators.find( strTaskId ) != m_mapCreators.end() )
	{
		Log::instance().Trace( 95, "CMessageParser::GetRegisterCreator: �������М��ߜ������ޜ�����՜��� ������؜��� ������ޜ��ޜ��ќ�����՜��ݜ��؜��� %s", strTaskId.c_str() );
		return m_mapCreators[ strTaskId ];
	}
	return NULL;
}

SmartPtr< CTask > CMessageParser::Parse( CInPacket& Message )
{
	Log::instance().Trace( 95, "CMessageParser::Parse: �������ќ������М��ќ��ޜ�����ڜ��� ���Ҝ�����ޜ��Ԝ��������՜��Ӝ��� ���ߜ��М��ڜ��՜������" );
	SmartPtr< CTask > pTask;
	std::string strCommandId;
	Message.GetField( COMMAND_ID, strCommandId );
	CreateTaskCallBack fnCreator;
	if( ( fnCreator = GetRegisterCreator( strCommandId ) ) != NULL )
	{
		pTask = fnCreator( m_ServerHandler );
		pTask->Load( Message );
	}else
		Log::instance().Trace( 95, "CMessageParser::Parse: �������՜��؜��ל��Ҝ��՜��������ݜ������ ������؜��� ������ޜ��ޜ��ќ�����՜��ݜ��؜���: %s", strCommandId.c_str() );
	return pTask;
}
