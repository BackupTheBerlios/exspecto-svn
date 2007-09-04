 //------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgent.h"
#include <process.h>
#include "ServerHandler.h"

//�������� ����� ����������
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
	//������������� ��������������� �����������
	int iLogLevel;
	Settings::instance().SetModule( "Agent", pAgentParamTypes, sizeof( pAgentParamTypes )/sizeof( pAgentParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );	

	Settings::instance().GetParam( SCHEDULER_ADDRESS, m_strSchedulerAddress );
	m_pMsgSock = SmartPtr< CServerSocket >( new CServerSocket() );
	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThreadProc, this, 0, NULL );
	//���� ����� �� �������� � ������� 2 � - ������������� ������ �������
	if( WAIT_TIMEOUT == WaitForSingleObject( m_hListenThread, 2000 ) )
		m_bStarted = true;
}

CAgent::~CAgent(void)
{
	try{
		m_CloseEvent.Set();
	
		m_pMsgSock->Close();
		Log::instance().Trace( 90, "CAgent::~CAgent: �������� �������� ������ �������������" );
		WaitForSingleObject( m_hListenThread, 10000 );
			
		CloseHandle( m_hListenThread );
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::~CAgent: �������� ����������: %s", e.what() );
	}catch(...)
	{
		Log::instance().Trace( 10," CAgent::ListenThread: �������� ����������� ����������" );
	}
}


//����� �������� �������� ����������
unsigned _stdcall CAgent::fnListenThreadProc(  void* pParameter )
{
	try{
		CAgent* pThis = (CAgent*)pParameter;
		
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;

		int iEventPort;
		Settings::instance().GetParam( SCHEDULER_EVENT_PORT, iEventPort );	
		Log::instance().Trace( 90, "CAgent:: ������ ������ �������� �������� ����������" );
	    //��������� ��������� ����� � ��������� �������
		int iListenPort;
		Settings::instance().GetParam( LISTEN_PORT, iListenPort );
		pThis->m_pMsgSock->Bind( iListenPort );
		//��������� ����� � ����� �������������
		pThis->m_pMsgSock->Listen();
		//������� �������� ���������� � ������������ ���
		while( NULL != ( client_sock = pThis->m_pMsgSock->Accept( adr ) ).get() )
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( pThis->m_CloseEvent, 0 ) )
				break;
			Log::instance().Trace( 51, "CAgent::ListenThread: �������� ���������� � ������: %s", adr.strAddr.c_str() );
			//��������� ���������� ������ �� ��������� ������� ������������
			if( pThis->m_strSchedulerAddress == adr.strAddr ) 
			{
				pThis->m_pEventSock = SmartPtr< CClientSocket >( new CClientSocket() );
				CServerHandler Handler( client_sock, pThis->m_pEventSock, pThis->m_strSchedulerAddress, iEventPort );
				pThis->m_vecConnections.push_back( SmartPtr< CConnectionHandler >( new CConnectionHandler( Handler ) ) );
			}else
				Log::instance().Trace( 50, "CAgent::ListenThread: �������� ���������� � ������: %s. ����������", adr.strAddr.c_str() );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: �������� ����������: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: �������� ����������� ����������" );
	}
	Log::instance().Trace( 50, "CAgent::ListenThread: ���������� ������ �������� �������� ���������" );
	return 0;
}