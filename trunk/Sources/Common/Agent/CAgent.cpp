//------------------------------------------------------------------------------------//
//Module: CAgent class
//Author: Parshin Dmitry
//Description: 
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgent.h"
#include "ServerHandler.h"

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

CAgent::CAgent():m_bStarted( false )
{
  int iLogLevel;
  Settings::instance().SetModule( "Agent", pAgentParamTypes, sizeof( pAgentParamTypes )/sizeof( pAgentParamTypes[0] ) );
  Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
  Log::instance().SetLoglevel( iLogLevel );

  Settings::instance().GetParam( SCHEDULER_ADDRESS, m_strSchedulerAddress );
  m_pMsgSock = SmartPtr< CServerSocket >( new CServerSocket() );

  m_pListenThread = SmartPtr<CThread>( new CThread( ( SmartPtr<CThreadTask>( new CListenThreadTask( this ) ) ) ) );
	
  Sleep(3000);
  m_bStarted = m_pListenThread->IsWorking();
}

CAgent::~CAgent(void)
{
  try
	{
	  m_pMsgSock->Close();
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

void CAgent::CListenThreadTask::Execute( CEvent& CancelEv )
{
  try
	{
	  Log::instance().Trace( 90, "CAgent::%s", __FUNCTION__ );

	  SmartPtr< CSocket > client_sock;

	  CServerSocket::structAddr adr;

	  int iEventPort;
	  Settings::instance().GetParam( SCHEDULER_EVENT_PORT, iEventPort );

	  int iListenPort;
	  Settings::instance().GetParam( LISTEN_PORT, iListenPort );

	  m_pAgent->m_pMsgSock->Bind( iListenPort );
	  m_pAgent->m_pMsgSock->Listen();

	  while( NULL != ( client_sock = m_pAgent->m_pMsgSock->Accept( adr ) ).get() )
		{
		  if( CancelEv.TryWait() )
			break;

		  Log::instance().Trace( 51, "%s: Accepted connection from : %s", __FUNCTION__, adr.strAddr.c_str() );

		  if( m_pAgent->m_strSchedulerAddress == adr.strAddr )
			{
			  m_pAgent->m_pEventSock = SmartPtr< CClientSocket >( new CClientSocket() );
			  CServerHandler Handler( client_sock, m_pAgent->m_pEventSock, m_pAgent->m_strSchedulerAddress, iEventPort );
			  m_pAgent->m_vecConnections.push_back( SmartPtr< CConnectionHandler >( new CConnectionHandler( Handler ) ) );
			}
		  else
			Log::instance().Trace( 50, "%s:Address %s is not in settings file", __FUNCTION__, adr.strAddr.c_str() );
		}
	}
  catch( std::exception& e )
	{
	  Log::instance().Trace( 10," %s: %s", __FUNCTION__, e.what() );
	}
  catch(...)
	{
	  Log::instance().Trace( 10,"%s: Unknown exception", __FUNCTION__ );
	}
  Log::instance().Trace( 50, "%s: end", __FUNCTION__ );

}

