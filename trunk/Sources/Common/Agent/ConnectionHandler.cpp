#include "precomp.h"
#include "ConnectionHandler.h"
#include <process.h>

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CConnectionHandler--------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CConnectionHandler::CConnectionHandler( CServerHandler& Handler ):m_ServerHandler( Handler )
													 			 ,m_MessageParser( Handler )
{
	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThread, this, 0, NULL );
}	
					
CConnectionHandler::~CConnectionHandler()
{
	try{
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: �������� ���������� � %s", m_ServerHandler.GetServerAddress().c_str() );
		m_CloseEv.Set();
		m_ServerHandler.CloseSession();
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: �������� �������� ������ �������������" );
		WaitForSingleObject( m_hListenThread, 10000 );
		CloseHandle( m_hListenThread );
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: �����������" );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::~CAgent: �������� ����������: %s", e.what() );
	}catch(...)
	{
		Log::instance().Trace( 10," CAgent::ListenThread: �������� ����������� ����������" );
	}		
}

unsigned _stdcall CConnectionHandler::fnListenThread( void* param )
{
	CConnectionHandler* pThis = (CConnectionHandler*)param;
	CInPacket Msg;

	try{
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: ������ ������ �������� �������� ��������� c ������ %s", pThis->m_ServerHandler.GetServerAddress().c_str() );
		while( true )
		{
			pThis->m_ServerHandler.Receive( Msg );
			Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: ������� �����" );
			//������ ������ ��� ������� ��������� ������
			SmartPtr< CTask > pTask = pThis->m_MessageParser.Parse( Msg );
			pThis->m_TaskHandler.AddTask( pTask );
		}
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: �������� ������ �������� �������� ��������� � ������ %s", pThis->m_ServerHandler.GetServerAddress().c_str() );
	}catch( SocketErr& e )
	{
		//pThis->m_ServerHandler.CloseSession();
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: ������ �����: %s", e.what() );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: �������� ����������: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: �������� ����������� ����������" );
	}
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: ��������" );
	return 0;	
}
