 //------------------------------------------------------------------------------------//
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: �������ۜ��М������, �������՜��М��ۜ��؜��ל�����������؜��� ���������ݜ��ڜ�����؜��� �������Ӝ��՜��ݜ������
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgent.h"
#include "ServerHandler.h"

//�������ߜ��؜�����М��ݜ��؜��� ������؜��ߜ��ޜ��� ���ߜ��М������М��ܜ��՜���������ޜ���
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
	//�������ݜ��؜�����؜��М��ۜ��؜��ל��М�����؜��� ���Ҝ�����ߜ��ޜ��ܜ��ޜ��Ӝ��М�����՜��ۜ�����ݜ������ ���ڜ��ޜ��ܜ��ߜ��ޜ��ݜ��՜��ݜ�����ޜ���
	int iLogLevel;
	Settings::instance().SetModule( "Agent", pAgentParamTypes, sizeof( pAgentParamTypes )/sizeof( pAgentParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );

	Settings::instance().GetParam( SCHEDULER_ADDRESS, m_strSchedulerAddress );
	m_pMsgSock = SmartPtr< CServerSocket >( new CServerSocket() );

	//����������ۜ��� ���ߜ��ޜ�����ޜ��� ���ݜ��� ���ל��М��ڜ���������ۜ������ ���� ������՜�����՜��ݜ��؜��� 2 ���� - ���؜��ݜ��؜�����؜��М��ۜ��؜��ל��М�����؜��� ���ߜ������ޜ�����ۜ��� ���������ߜ��՜�����ݜ���
	Sleep(2000);
    m_bStarted = m_ListenThread.IsWorking();
}

CAgent::~CAgent(void)
{
	try{
		m_pMsgSock->Close();
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::~CAgent: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���: %s", e.what() );
	}catch(...)
	{
		Log::instance().Trace( 10," CAgent::ListenThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���" );
	}
}

//�������ޜ�����ޜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� ���Ҝ�����ޜ��Ԝ��������؜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜���
void CAgent::CListenThreadTask::Execute( CEvent& CancelEv )
{
	try{
		SmartPtr< CSocket > client_sock;

		CServerSocket::structAddr adr;

		int iEventPort;
		Settings::instance().GetParam( SCHEDULER_EVENT_PORT, iEventPort );
		Log::instance().Trace( 90, "CAgent:: �������М��ߜ��������� ���ߜ��ޜ�����ޜ��ڜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� ���Ҝ�����ޜ��Ԝ��������؜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜���" );
	    //������Ҝ�����ל�����Ҝ��М��՜��� ������՜������Ҝ��՜������ݜ������ ������ޜ��ڜ��՜��� ���� ���ۜ��ޜ��ڜ��М��ۜ�����ݜ������ ���М��Ԝ������՜�����ޜ���
		int iListenPort;
		Settings::instance().GetParam( LISTEN_PORT, iListenPort );
		m_pAgent->m_pMsgSock->Bind( iListenPort );
		//���ߜ��՜������՜��Ҝ��ޜ��Ԝ��؜��� ������ޜ��ڜ��՜��� ���� �������՜��֜��؜��� ���ߜ������ޜ�����ۜ��������؜��Ҝ��М��ݜ��؜���
		m_pAgent->m_pMsgSock->Listen();
		//�������֜��؜��Ԝ��М��՜��� ���Ҝ�����ޜ��Ԝ��������՜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� ���� ���ޜ��ќ������М��ќ��М��������Ҝ��М��՜��� ���՜��Ӝ���
		while( NULL != ( client_sock = m_pAgent->m_pMsgSock->Accept( adr ) ).get() )
		{
		    if( CancelEv.TryWait() )
                break;
			Log::instance().Trace( 51, "CAgent::ListenThread: ����������ޜ��Ԝ��������՜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� ���� ���М��Ԝ������՜������: %s", adr.strAddr.c_str() );
			//���ߜ������؜��ݜ��؜��ܜ��М��՜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� ������ޜ��ۜ�����ڜ��� ���ޜ��� ���ל��М��Ԝ��М��ݜ��ݜ��ޜ��Ӝ��� ������՜������Ҝ��՜������� ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���
			if( m_pAgent->m_strSchedulerAddress == adr.strAddr )
			{
				m_pAgent->m_pEventSock = SmartPtr< CClientSocket >( new CClientSocket() );
				CServerHandler Handler( client_sock, m_pAgent->m_pEventSock, m_pAgent->m_strSchedulerAddress, iEventPort );
				m_pAgent->m_vecConnections.push_back( SmartPtr< CConnectionHandler >( new CConnectionHandler( Handler ) ) );
			}else
				Log::instance().Trace( 50, "CAgent::ListenThread: ����������ޜ��Ԝ��������՜��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� ���� ���М��Ԝ������՜������: %s. �������Ӝ��ݜ��ޜ������؜���������՜���", adr.strAddr.c_str() );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���" );
	}
	Log::instance().Trace( 50, "CAgent::ListenThread: �������М��Ҝ��՜���������՜��ݜ��؜��� ���ߜ��ޜ�����ޜ��ڜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� ���Ҝ�����ޜ��Ԝ��������؜��� ������ޜ��ޜ��ќ�����՜��ݜ��؜���" );
}

