 //------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#include "CAgent.h"
#include <process.h>

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------CAgent------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CAgent::CAgent()
{
	Settings::instance().GetParam( SCHEDULER_ADDRESS, m_strSchedulerAddress );
	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThreadProc, this, 0, NULL );
}

CAgent::~CAgent(void)
{
	try{
		m_CloseEvent.Set();
	
		m_sock.Close();
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
	
		Log::instance().Trace( 90, "CAgent:: ������ ������ �������� �������� ����������" ); 
	    //��������� ��������� ����� � ��������� �������
		pThis->m_sock.Bind( 5000, "127.0.0.1" );
		//��������� ����� � ����� �������������
		pThis->m_sock.Listen();
		//������� �������� ���������� � ������������ ���
		while( NULL != ( client_sock = pThis->m_sock.Accept( adr ) ).get() )
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( pThis->m_CloseEvent, 0 ) )
				break;
			Log::instance().Trace( 51, "CAgent::ListenThread: �������� ���������� � ������: %s", adr.strAddr.c_str() );
			//��������� ���������� ������ �� ��������� ������� ������������
			if( pThis->m_strSchedulerAddress == adr.strAddr ) 
			{
				CServerHandler Handler( SmartPtr< CSocket >( &pThis->m_sock ), SmartPtr< CSocket >( &pThis->m_EventSock ) );
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






