#include "MessageParser.h"
#include "CTask.h"

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CMessageParser------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
	
CMessageParser::CreateTaskCallBack CMessageParser::GetRegisterCreator( int iTaskId, CreateTaskCallBack fnCreator )
{
	static std::map< int, CreateTaskCallBack > m_mapCreators;
	if( NULL != fnCreator )
	{
		Log::instance().Trace( 95, "CMessageParser::GetRegisterCreator: ��������������� ��� ��������� %d", iTaskId );
		m_mapCreators[ iTaskId ] = fnCreator;
	}else if( m_mapCreators.find( iTaskId ) != m_mapCreators.end() )
	{
		Log::instance().Trace( 95, "CMessageParser::GetRegisterCreator: �������� ��� ��������� %d", iTaskId );
		return m_mapCreators[ iTaskId ];
	}
	return NULL;
}

std::vector< SmartPtr< CTask > > CMessageParser::Parse( CPacket& Message )
{
	Log::instance().Trace( 95, "CMessageParser::Parse: ��������� ��������� ������" );
	BYTE bCommandId;
	std::vector< SmartPtr< CTask > > vecRes;
	SmartPtr< CTask > pTask;
	for(;;)
	{
		if( !Message.IsDone() )	
			Message.GetCommandId( bCommandId );
		else
		{
			Log::instance().Trace( 95, "CMessageParser::Parse: ��������� ��������� ������ ���������.����� ������:%d", vecRes.size() ); 
			break;
		}
		CreateTaskCallBack fnCreator;
		if( ( fnCreator = GetRegisterCreator( bCommandId ) ) != NULL )
		{
			CTask* pTask = fnCreator( m_ServerHandler );
			pTask->Load( Message );
			vecRes.push_back( pTask );
		}else
			Log::instance().Trace( 95, "CMessageParser::Parse: ����������� ��� ���������: %d", bCommandId );
	}
	return vecRes;
}
