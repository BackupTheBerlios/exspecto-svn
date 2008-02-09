#include "precomp.h"
#include "ConnectionHandler.h"

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CConnectionHandler--------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CConnectionHandler::CConnectionHandler( CServerHandler& Handler ):m_ServerHandler( Handler )
													 			 ,m_MessageParser( Handler )
                                                                 ,m_ListenThread( SmartPtr<CThreadTask>( new CListenThreadTask( this ) ) )
{
}

CConnectionHandler::~CConnectionHandler()
{
  try
	{
	  Log::instance().Trace( 90, "%s: Scheduler address %s", __FUNCTION__, m_ServerHandler.GetServerAddress().c_str() );
	  m_ServerHandler.CloseSession();
	}
  catch( std::exception& e )
	{
	  Log::instance().Trace( 10," %s: %s", __FUNCTION__, e.what() );
	}
  catch(...)
	{
	  Log::instance().Trace( 10,"%s: Unknown exception", __FUNCTION__ );
	}
}

void CConnectionHandler::CListenThreadTask::Execute( CEvent& CancelEv )
{
  CInPacket Msg;

  try
	{

	  Log::instance().Trace( 90, "%s: Scheduler address %s", __FUNCTION__, m_pHandler->m_ServerHandler.GetServerAddress().c_str() );
		
	  while( true )
		{
		  m_pHandler->m_ServerHandler.Receive( Msg );

		  Log::instance().Trace( 90, "%s: Message received", __FUNCTION__ );

		  SmartPtr< CTask > pTask = m_pHandler->m_MessageParser.Parse( Msg );

		  m_pHandler->m_TaskHandler.AddTask( pTask );
		}

	  Log::instance().Trace( 90, "%s: Scheduler address  %s", __FUNCTION__, m_pHandler->m_ServerHandler.GetServerAddress().c_str() );
		
	}
  catch( SocketErr& e )
	{
	  Log::instance().Trace( 10," %s: connection lost: %s", __FUNCTION__, e.what() );
	}
  catch( std::exception& e )
	{
	  Log::instance().Trace( 10," %s: %s", __FUNCTION__, e.what() );
	}
  catch(...)
	{
	  Log::instance().Trace( 10,"%s: Unknown exception", __FUNCTION__ );
	}
	
  Log::instance().Trace( 90, "%s: end", __FUNCTION__ );
}


