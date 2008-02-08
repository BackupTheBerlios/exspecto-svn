 //------------------------------------------------------------------------------------//
//œô¨Íœô¨âœô¨Þœô¨â œô¨äœô¨Ðœô¨Ùœô¨Û œô¨ïœô¨Òœô¨Ûœô¨ïœô¨Õœô¨âœô¨áœô¨ï œô¨çœô¨Ðœô¨áœô¨âœô¨ìœô¨î œô¨ßœô¨àœô¨Þœô¨Õœô¨Úœô¨âœô¨Ð Exspecto 2006œô¨Ó.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: œô¨ºœô¨Ûœô¨Ðœô¨áœô¨á, œô¨àœô¨Õœô¨Ðœô¨Ûœô¨Øœô¨×œô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨Ø œô¨°œô¨Óœô¨Õœô¨Ýœô¨âœô¨Ð
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgent.h"
#include "ServerHandler.h"

//œô¨¾œô¨ßœô¨Øœô¨áœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨âœô¨Øœô¨ßœô¨Þœô¨Ò œô¨ßœô¨Ðœô¨àœô¨Ðœô¨Üœô¨Õœô¨âœô¨àœô¨Þœô¨Ò
static char* pAgentParamTypes[] = {
	SCHEDULER_ADDRESS, "string",
	LOG_LEVEL,	"int",
	EVENT_PORT, "int",
	SCAN_THREADS_COUNT, "int",
	PING_ON, "bool",
	SCHEDULER_EVENT_PORT, "int",
	LISTEN_PORT, "int",
	RESOLVE_HOST, "bool"
};
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------CAgent------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CAgent::CAgent():m_bStarted( false ),m_ListenThread( SmartPtr<CThreadTask>( new CListenThreadTask( this ) ) )
{
	//œô¨¸œô¨Ýœô¨Øœô¨æœô¨Øœô¨Ðœô¨Ûœô¨Øœô¨×œô¨Ðœô¨æœô¨Øœô¨ï œô¨Òœô¨áœô¨ßœô¨Þœô¨Üœô¨Þœô¨Óœô¨Ðœô¨âœô¨Õœô¨Ûœô¨ìœô¨Ýœô¨ëœô¨å œô¨Úœô¨Þœô¨Üœô¨ßœô¨Þœô¨Ýœô¨Õœô¨Ýœô¨âœô¨Þœô¨Ò
	int iLogLevel;
	Settings::instance().SetModule( "Agent", pAgentParamTypes, sizeof( pAgentParamTypes )/sizeof( pAgentParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );

	Settings::instance().GetParam( SCHEDULER_ADDRESS, m_strSchedulerAddress );
	m_pMsgSock = SmartPtr< CServerSocket >( new CServerSocket() );

	//œô¨µœô¨áœô¨Ûœô¨Ø œô¨ßœô¨Þœô¨âœô¨Þœô¨Ú œô¨Ýœô¨Õ œô¨×œô¨Ðœô¨Úœô¨àœô¨ëœô¨Ûœô¨áœô¨ï œô¨Ò œô¨âœô¨Õœô¨çœô¨Õœô¨Ýœô¨Øœô¨Ø 2 œô¨á - œô¨Øœô¨Ýœô¨Øœô¨æœô¨Øœô¨Ðœô¨Ûœô¨Øœô¨×œô¨Ðœô¨æœô¨Øœô¨ï œô¨ßœô¨àœô¨Þœô¨èœô¨Ûœô¨Ð œô¨ãœô¨áœô¨ßœô¨Õœô¨èœô¨Ýœô¨Þ
	Sleep(2000);
    m_bStarted = m_ListenThread.IsWorking();
}

CAgent::~CAgent(void)
{
	try{
		m_pMsgSock->Close();
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::~CAgent: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ: %s", e.what() );
	}catch(...)
	{
		Log::instance().Trace( 10," CAgent::ListenThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ" );
	}
}

//œô¨¿œô¨Þœô¨âœô¨Þœô¨Ú œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Øœô¨å œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Ù
void CAgent::CListenThreadTask::Execute( CEvent& CancelEv )
{
	try{
		SmartPtr< CSocket > client_sock;

		CServerSocket::structAddr adr;

		int iEventPort;
		Settings::instance().GetParam( SCHEDULER_EVENT_PORT, iEventPort );
		Log::instance().Trace( 90, "CAgent:: œô¨·œô¨Ðœô¨ßœô¨ãœô¨áœô¨Ú œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Ð œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Øœô¨å œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Ù" );
	    //œô¨áœô¨Òœô¨ïœô¨×œô¨ëœô¨Òœô¨Ðœô¨Õœô¨Ü œô¨áœô¨Õœô¨àœô¨Òœô¨Õœô¨àœô¨Ýœô¨ëœô¨Ù œô¨áœô¨Þœô¨Úœô¨Õœô¨â œô¨á œô¨Ûœô¨Þœô¨Úœô¨Ðœô¨Ûœô¨ìœô¨Ýœô¨ëœô¨Ü œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Þœô¨Ü
		int iListenPort;
		Settings::instance().GetParam( LISTEN_PORT, iListenPort );
		m_pAgent->m_pMsgSock->Bind( iListenPort );
		//œô¨ßœô¨Õœô¨àœô¨Õœô¨Òœô¨Þœô¨Ôœô¨Øœô¨Ü œô¨áœô¨Þœô¨Úœô¨Õœô¨â œô¨Ò œô¨àœô¨Õœô¨Öœô¨Øœô¨Ü œô¨ßœô¨àœô¨Þœô¨áœô¨Ûœô¨ãœô¨èœô¨Øœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï
		m_pAgent->m_pMsgSock->Listen();
		//œô¨¾œô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Õœô¨Ü œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Õœô¨Õ œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Ø œô¨Þœô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Ðœô¨âœô¨ëœô¨Òœô¨Ðœô¨Õœô¨Ü œô¨Õœô¨Óœô¨Þ
		while( NULL != ( client_sock = m_pAgent->m_pMsgSock->Accept( adr ) ).get() )
		{
		    if( CancelEv.TryWait() )
                break;
			Log::instance().Trace( 51, "CAgent::ListenThread: œô¨²œô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Õœô¨Õ œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨á œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð: %s", adr.strAddr.c_str() );
			//œô¨ßœô¨àœô¨Øœô¨Ýœô¨Øœô¨Üœô¨Ðœô¨Õœô¨Ü œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨ï œô¨âœô¨Þœô¨Ûœô¨ìœô¨Úœô¨Þ œô¨Þœô¨â œô¨×œô¨Ðœô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨Þœô¨Óœô¨Þ œô¨áœô¨Õœô¨àœô¨Òœô¨Õœô¨àœô¨Ð œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï
			if( m_pAgent->m_strSchedulerAddress == adr.strAddr )
			{
				m_pAgent->m_pEventSock = SmartPtr< CClientSocket >( new CClientSocket() );
				CServerHandler Handler( client_sock, m_pAgent->m_pEventSock, m_pAgent->m_strSchedulerAddress, iEventPort );
				m_pAgent->m_vecConnections.push_back( SmartPtr< CConnectionHandler >( new CConnectionHandler( Handler ) ) );
			}else
				Log::instance().Trace( 50, "CAgent::ListenThread: œô¨²œô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Õœô¨Õ œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨á œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð: %s. œô¨¸œô¨Óœô¨Ýœô¨Þœô¨àœô¨Øœô¨àœô¨ãœô¨Õœô¨Ü", adr.strAddr.c_str() );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ" );
	}
	Log::instance().Trace( 50, "CAgent::ListenThread: œô¨·œô¨Ðœô¨Òœô¨Õœô¨àœô¨èœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨ßœô¨Þœô¨âœô¨Þœô¨Úœô¨Ð œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Øœô¨å œô¨áœô¨Þœô¨Þœô¨Ñœô¨éœô¨Õœô¨Ýœô¨Øœô¨Ù" );
}

