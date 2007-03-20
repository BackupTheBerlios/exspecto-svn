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
#include <process.h>

CScheduler::CScheduler(void)
{
	//��������� ��������� �������
	m_mapAgentsContainer[ "127.0.0.1" ] = SmartPtr< CAgentHandler >( new CAgentHandler( "127.0.0.1" ) ); 
	
	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThreadProc, this, 0, NULL );
		
	Log::instance().Trace( 90, "CScheduler: ��������, �������� ������" );
	m_pTrigger = std::auto_ptr< CTimer >( new CTimer( this ) );
	m_pTrigger->Start();
	Sleep(8000);
	m_pTrigger->Stop();
}

CScheduler::~CScheduler(void)
{
	m_CloseEv.Set();
	if( m_EventSock.IsConnected() )
	{
		Log::instance().Trace( 90, "CScheduler::~CScheduler: ����� ������� ������" );
		m_EventSock.Close();
	}
	else
		Log::instance().Trace( 90, "CScheduler::~CScheduler: ����� ������� ������" );

	Log::instance().Trace( 90, "CScheduler::~CScheduler: �������� �������� ������ �������������" );
	WaitForSingleObject( m_hListenThread, 10000 );
	Log::instance().Trace( 90, "CScheduler::~CScheduler: 2" );
	CloseHandle( m_hListenThread );
}

void CScheduler::OnStartScan()
{
	std::vector< std::string > vecAdr;
	vecAdr.push_back( "127.0.0.1" );
	for( std::map< std::string, SmartPtr< CAgentHandler > >::iterator It = m_mapAgentsContainer.begin(); It != m_mapAgentsContainer.end(); It++ )
	{
/*		enumAgentState bStatus;
		It->second->Open();
		if( It->second->IsOpened() )
		{
			It->second->GetStatus( bStatus );
			Log::instance().Trace( 10, "CScheduler: ������ ������: %d", bStatus );
		}
*/		
		It->second->Open();
		if( It->second->IsOpened() )
		{
			It->second->BeginScan( vecAdr );
			//Log::instance().Trace( 10, "CScheduler: ������ ������: %d", bStatus );
		}
		Sleep( 2000 );
		It->second->Open();
		if( It->second->IsOpened() )
		{
			It->second->GetData();
			//Log::instance().Trace( 10, "CScheduler: ������ ������: %d", bStatus );
		}
	}
}

//����� �������� �������� ����������
unsigned _stdcall CScheduler::fnListenThreadProc(  void* pParameter )
{
	try{
		CScheduler* pThis = (CScheduler*)pParameter;
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CScheduler::fnListenThreadProc: ������ ������ �������� �������� ����������" ); 
	    //��������� ��������� ����� � ��������� �������
		pThis->m_EventSock.Bind( 3000, "127.0.0.1" );
		//��������� ����� � ����� �������������
		pThis->m_EventSock.Listen();
		//������� �������� ���������� � ������������ ���
		while( ( NULL != ( client_sock = pThis->m_EventSock.Accept( adr ) ).get() ) && ( WAIT_OBJECT_0 != WaitForSingleObject( pThis->m_CloseEv, 0 ) ) )
		{
			Log::instance().Trace( 51, "CScheduler::ListenThread: �������� ���������� � ������: %s", adr.strAddr.c_str() );
			try{
				//��������� ���������� ������ �� ��������� ������� ������������
				if( pThis->m_mapAgentsContainer.find( adr.strAddr ) != pThis->m_mapAgentsContainer.end() ) 
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
