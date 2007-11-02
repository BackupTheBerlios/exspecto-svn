#include "precomp.h"
#include "ConnectionHandler.h"
#include <process.h>

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CConnectionHandler--------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CConnectionHandler::CConnectionHandler( CServerHandler& Handler ):m_ServerHandler( Handler )
													 			 ,m_MessageParser( Handler )
                                                                 ,m_ListenThread( SmartPtr<CListenThreadTask( this ) )
{
}

CConnectionHandler::~CConnectionHandler()
{
	try{
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: �������� ���������� � %s", m_ServerHandler.GetServerAddress().c_str() );
		m_ServerHandler.CloseSession();
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: �����������" );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::~CAgent: �������� ����������: %s", e.what() );
	}catch(...)
	{
		Log::instance().Trace( 10," CAgent::ListenThread: �������� ����������� ����������" );
	}
}

void CConnectionHandler::CListenThreadTask::Execute( const CEvent& CancelEv )
{
	CInPacket Msg;

	try{
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: ������ ������ �������� �������� ��������� c ������ %s", m_pHandler->m_ServerHandler.GetServerAddress().c_str() );
		while( true )
		{
			m_pHandler->m_ServerHandler.Receive( Msg );
			Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: ������� �����" );
			//������ ������ ��� ������� ��������� ������
			SmartPtr< CTask > pTask = m_pHandler->m_MessageParser.Parse( Msg );
			m_pHandler->m_TaskHandler.AddTask( pTask );
		}
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: �������� ������ �������� �������� ��������� � ������ %s", m_pHandler->m_ServerHandler.GetServerAddress().c_str() );
	}catch( SocketErr& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: ������ �����: %s", e.what() );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: �������� ����������: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: �������� ����������� ����������" );
	}
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: ��������" );
}


