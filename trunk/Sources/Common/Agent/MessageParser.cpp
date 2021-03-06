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
	  Log::instance().Trace( 95, "%s: Registering creator for task id - %s", __FUNCTION__, strTaskId.c_str() );
	  m_mapCreators[ strTaskId ] = fnCreator;
	}
  else if( m_mapCreators.find( strTaskId ) != m_mapCreators.end() )
	{
	  Log::instance().Trace( 95, "%s: Returning creator for task id =  %s", __FUNCTION__, strTaskId.c_str() );
	  return m_mapCreators[ strTaskId ];
	}
  return NULL;
}

SmartPtr< CTask > CMessageParser::Parse( CInPacket& Message )
{
  Log::instance().Trace( 95, "%s:", __FUNCTION__ );
  SmartPtr< CTask > pTask;
  std::string strCommandId;
  Message.GetField( COMMAND_ID, strCommandId );
  CreateTaskCallBack fnCreator;
  if( ( fnCreator = GetRegisterCreator( strCommandId ) ) != NULL )
	{
	  pTask = fnCreator( m_ServerHandler );
	  pTask->Load( Message );
	}else
	Log::instance().Trace( 95, "%s: Unknown command: %s", __FUNCTION__, strCommandId.c_str() );
  return pTask;
}
