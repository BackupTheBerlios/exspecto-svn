//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������
//-------------------------------------------------------------------------------------//
#include <iostream>
#include <tchar.h>
#include "CScheduler.h"
#include "windows.h"
#include "ServerSocket.h"

CScheduler::CScheduler(void)
{
	//��������� ��������� �������
	m_mapAgentsContainer[ "127.0.0.1" ] = SmartPtr< CAgentHandler >( new CAgentHandler( "127.0.0.1" ) ); 
	
	Log::instance().Trace( 90, "CScheduler: ��������, �������� ������" );
	m_pTrigger = std::auto_ptr< CTimer >( new CTimer( this ) );
	m_pTrigger->Start();
	Sleep(8000);
	m_pTrigger->Stop();
}

CScheduler::~CScheduler(void){}

void CScheduler::OnStartScan()
{
	std::vector< std::string > vecAdr;
	vecAdr.push_back( "127.0.0.1" );
	for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = m_mapAgentsContainer.begin(); It != m_mapAgentsContainer.end(); It++ )
	{
		enumAgentState bStatus;
		It->second->Open();
		if( It->second->IsOpened() )
		{
			It->second->GetStatus( bStatus );
			Log::instance().Trace( 10, "CScheduler: ������ ������: %d", bStatus );
		}
		It->second->Open();
		if( It->second->IsOpened() )
		{
			Log::instance().Trace( 10, "CScheduler: ����� ������: %d", It->second->BeginScan( vecAdr ) );
		}
		It->second->Open();
		if( It->second->IsOpened() )
		{
			It->second->GetStatus( bStatus );
			Log::instance().Trace( 10, "CScheduler: ������ ������: %d", bStatus );
		}
		It->second->Open();
		if( It->second->IsOpened() )
		{
			Log::instance().Trace( 10, "CScheduler: ����� ������: %d", It->second->StopScan() );
		}
		
	}
}

//����� �������� �������� ����������
unsigned _stdcall CScheduler::fnListenThreadProc(  void* pParameter )
{
	try{
		CScheduler* pThis = (CScheduler*)pParameter;
		CServerSocket sock;
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CScheduler:: ������ ������ �������� �������� ����������" ); 
	    //��������� ��������� ����� � ��������� �������
		sock.Bind( 3000, "127.0.0.1" );
		//��������� ����� � ����� �������������
		sock.Listen();
		//������� �������� ���������� � ������������ ���
		while( ( NULL != ( client_sock = sock.Accept( adr ) ).get() ) && ( WAIT_OBJECT_0 != WaitForSingleObject( pThis->m_CloseEv, 0 ) ) )
		{
			Log::instance().Trace( 51, "CScheduler::ListenThread: �������� ���������� � ������: %s", adr.strAddr.c_str() );
			try{
				//��������� ���������� ������ �� ��������� ������� ������������
				if( m_mapAgentsContainer.find( adr.strAddr ) != m_mapAgentsContainer.end() ) 
					pThis->m_mapAgentsContainer[ adr.strAddr ]->OnConnection( client_sock );
				else
					Log::instance().Trace( 50, "CScheduler::ListenThread: ������ ����� � ������: %s. ����������" );
			}catch( CSocket::SocketErr& e )
			{
				Log::instance().Trace( 50, "CScheduler::ListenThread: ���������� socket: %s", e.what() );
				//���� �������� ����� ������ 10��
				continue;
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CScheduler::ListenThread: �������� ����������: %s", e.what() );
	}
	Log::instance().Trace( 50, "CScheduler::ListenThread: ���������� ������ �������� �������� ���������" );
	return 0;
}
