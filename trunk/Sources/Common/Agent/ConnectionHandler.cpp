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
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: �������М��ڜ������������؜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� ���� %s", m_ServerHandler.GetServerAddress().c_str() );
		m_ServerHandler.CloseSession();
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: ���Ü��ݜ��؜��������ޜ��֜��՜��ݜ��؜���" );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::~CAgent: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���: %s", e.what() );
	}catch(...)
	{
		Log::instance().Trace( 10," CAgent::ListenThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���" );
	}
}

void CConnectionHandler::CListenThreadTask::Execute( CEvent& CancelEv )
{
	CInPacket Msg;

	try{
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: �������М��ߜ��������� ���ߜ��ޜ�����ޜ��ڜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� ���Ҝ�����ޜ��Ԝ��������؜��� ������ޜ��ޜ��ќ�����՜��ݜ��؜��� c ���М��Ԝ������՜������ %s", m_pHandler->m_ServerHandler.GetServerAddress().c_str() );
		while( true )
		{
			m_pHandler->m_ServerHandler.Receive( Msg );
			Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: �������ޜ��ۜ��������՜��� ���ߜ��М��ڜ��՜���" );
			//�������М��Ԝ��М��՜��� ���Ԝ��М��ݜ��ݜ������ ���Ԝ��ۜ��� �������М��ל��ќ��ޜ������� ���Ҝ�����ޜ��Ԝ��������՜��Ӝ��� ���ߜ��М��ڜ��՜������
			SmartPtr< CTask > pTask = m_pHandler->m_MessageParser.Parse( Msg );
			m_pHandler->m_TaskHandler.AddTask( pTask );
		}
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: �������М��ڜ������������؜��� ���ߜ��ޜ�����ޜ��ڜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� ���Ҝ�����ޜ��Ԝ��������؜��� ������ޜ��ޜ��ќ�����՜��ݜ��؜��� ���� ���М��Ԝ������՜������ %s", m_pHandler->m_ServerHandler.GetServerAddress().c_str() );
	}catch( SocketErr& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: ����������؜��ќ��ڜ��� ������Ҝ�����ל���: %s", e.what() );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���" );
	}
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: �������М��ڜ������������؜���" );
}


