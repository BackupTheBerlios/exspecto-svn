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
	CPacket Msg;

	try{
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: ������ ������ �������� �������� ��������� c ������ %s", pThis->m_ServerHandler.GetServerAddress().c_str() );
		while( true )
		{
			pThis->m_ServerHandler.Receive( Msg );
			BYTE* pbBuf;
			int iSize = 0;
			Msg.GetBuffer( pbBuf, iSize );
			Log::instance().Dump( 90, pbBuf, iSize, "CConnectionHandler::fnListenThread: ������� �����: " );
			//������ ������ ��� ������� ��������� ������
			std::vector< SmartPtr< CTask > > vecTasks = pThis->m_MessageParser.Parse( Msg );
			for( std::vector< SmartPtr< CTask > >::iterator It = vecTasks.begin(); It != vecTasks.end(); It++ )
				pThis->m_TaskHandler.AddTask( *It );
		}
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: �������� ������ �������� �������� ��������� � ������ %s", pThis->m_ServerHandler.GetServerAddress().c_str() );
	}catch( SocketErr& e )
	{
		pThis->m_ServerHandler.CloseSession();
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
