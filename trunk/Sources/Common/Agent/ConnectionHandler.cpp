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
	try{
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: œô¨·œô¨Ðœô¨Úœô¨àœô¨ëœô¨âœô¨Øœô¨Õ œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨ï œô¨á %s", m_ServerHandler.GetServerAddress().c_str() );
		m_ServerHandler.CloseSession();
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: œô¨Ãœô¨Ýœô¨Øœô¨çœô¨âœô¨Þœô¨Öœô¨Õœô¨Ýœô¨Øœô¨Õ" );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::~CAgent: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ: %s", e.what() );
	}catch(...)
	{
		Log::instance().Trace( 10," CAgent::ListenThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ" );
	}
}

void CConnectionHandler::CListenThreadTask::Execute( CEvent& CancelEv )
{
	CInPacket Msg;

	try{
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: œô¨·œô¨Ðœô¨ßœô¨ãœô¨áœô¨Ú œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Ð œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Øœô¨å œô¨áœô¨Þœô¨Þœô¨Ñœô¨éœô¨Õœô¨Ýœô¨Øœô¨Ù c œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð %s", m_pHandler->m_ServerHandler.GetServerAddress().c_str() );
		while( true )
		{
			m_pHandler->m_ServerHandler.Receive( Msg );
			Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: œô¨¿œô¨Þœô¨Ûœô¨ãœô¨çœô¨Õœô¨Ý œô¨ßœô¨Ðœô¨Úœô¨Õœô¨â" );
			//œô¨·œô¨Ðœô¨Ôœô¨Ðœô¨Õœô¨Ü œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨Õ œô¨Ôœô¨Ûœô¨ï œô¨àœô¨Ðœô¨×œô¨Ñœô¨Þœô¨àœô¨Ð œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Õœô¨Óœô¨Þ œô¨ßœô¨Ðœô¨Úœô¨Õœô¨âœô¨Ð
			SmartPtr< CTask > pTask = m_pHandler->m_MessageParser.Parse( Msg );
			m_pHandler->m_TaskHandler.AddTask( pTask );
		}
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: œô¨·œô¨Ðœô¨Úœô¨àœô¨ëœô¨âœô¨Øœô¨Õ œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Ð œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Øœô¨å œô¨áœô¨Þœô¨Þœô¨Ñœô¨éœô¨Õœô¨Ýœô¨Øœô¨Ù œô¨á œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð %s", m_pHandler->m_ServerHandler.GetServerAddress().c_str() );
	}catch( SocketErr& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: œô¨¾œô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð œô¨áœô¨Òœô¨ïœô¨×œô¨Ø: %s", e.what() );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ" );
	}
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: œô¨·œô¨Ðœô¨Úœô¨àœô¨ëœô¨âœô¨Øœô¨Õ" );
}


