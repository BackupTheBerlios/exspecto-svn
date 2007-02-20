 //------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#include "CAgent.h"
#include "ServerSocket.h"
#include <process.h>
#include "SmartPtr.hpp"

enumAgentState CTask::m_CurState = Idling;
CCriticalSection CTask::m_csCurState;
CEvent CTask::m_CancelEv;
std::vector< std::string > CTask::m_vecData;

//�����������,strSchedulerAddress - ����� ������������
CAgent::CAgent()
{
	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThreadProc, this, 0, NULL );
}

CAgent::~CAgent(void)
{
	m_CloseEvent.Set();
	WaitForSingleObject( m_hListenThread, 10000 );
	Log::instance().Trace( 90, "CAgent::~CAgent: �������� ������" );
}


//����� �������� �������� ����������
unsigned _stdcall CAgent::fnListenThreadProc(  void* pParameter )
{
	try{
		CAgent* pThis = (CAgent*)pParameter;
		CServerSocket sock;
		SmartPtr< CSocket > client_sock;
	
		CServerSocket::structAddr adr;
	
		Log::instance().Trace( 90, "CAgent:: ������ ������ �������� �������� ����������" ); 
	    //��������� ��������� ����� � ��������� �������
		sock.Bind( 5000, "192.168.1.189" );
		//��������� ����� � ����� �������������
		sock.Listen();
		//������� �������� ���������� � ������������ ���
		while( ( NULL != ( client_sock = sock.Accept( adr ) ).get() ) && ( WAIT_OBJECT_0 != WaitForSingleObject( pThis->m_CloseEvent, 0 ) ) )
		{
			Log::instance().Trace( 51, "CAgent::ListenThread: �������� ���������� � ������: %s", adr.strAddr.c_str() );
			try{
				//��������� ���������� ������ �� ��������� ������� ������������
				if( pThis->m_strSchedulerAddress == adr.strAddr ) 
				{
					pThis->m_vecConnections.push_back( SmartPtr< CConnectionHandler >( new CConnectionHandler( client_sock ) ) );
				}else
					Log::instance().Trace( 50, "CAgent::ListenThread: ������ ����� � ������: %s. ����������" );
			}catch( CSocket::SocketErr& e )
			{
				Log::instance().Trace( 50, "CAgent::ListenThread: ���������� socket: %s", e.what() );
				//���� �������� ����� ������ 10��
				continue;
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CAgent::ListenThread: �������� ����������: %s", e.what() );
	}
	Log::instance().Trace( 50, "CAgent::ListenThread: ���������� ������ �������� �������� ���������" );
	return 0;
}

void CConnectionHandler::Listen()
{
	CPacket Msg;
	BYTE pBuf[10240];
	int iCount;
	while( iCount = m_pSocket->Receive( pBuf, 10240 ) )
	{
		//������ ������ ��� ������� ��������� ������
		Msg.SetBuffer( pBuf, iCount );
		std::vector< SmartPtr< CTask > > vecTasks = m_MessageParser.Parse( Msg );
		for( std::vector< SmartPtr< CTask > >::iterator It = vecTasks.begin(); It != vecTasks.end(); It++ )
			m_TaskHandler.AddTask( *It );
	}
}
	
SmartPtr< CTask > CMessageParser::TaskFactory( BYTE bCommandId, CPacket& Msg )
{
	switch( bCommandId )
	{
		case GET_STATUS:
			return new CGetStatus( m_pSocket );
		case START_SCAN:
		{
			DWORD dwCount;
			std::string strAddress;
			std::vector< std::string > vecAddresses;
			//�������� ���-�� ������� � ������
			Msg.GetParam( dwCount );
			Log::instance().Trace( 80, "CAgent:StartScan: ���-�� ������� ��� ������������: %d", dwCount );
			for( unsigned int i = 0; i < dwCount; i++ )
			{
				//�������� ��������� �����
				Msg.GetAddress( strAddress );
				vecAddresses.push_back( strAddress );
			}	
			return new CStartScan( m_pSocket, vecAddresses );
		}
		case GET_DATA:
			return new CGetData( m_pSocket );
		case STOP_SCAN:
			return new CStopScan( m_pSocket );
		default:
			return NULL;
	}
}

std::vector< SmartPtr< CTask > > CMessageParser::Parse( CPacket& Message )
{
	BYTE bCommandId;
	std::vector< SmartPtr< CTask > > vecRes;
	SmartPtr< CTask > pTask;
	for(;;)
	{
		if( !Message.IsDone() )	
			Message.GetCommandId( bCommandId );
		else
		{
			Log::instance().Trace( 95, "CAgent::fnProcessThreadProc: ��������� ��������� ������ ���������" ); 
			break;
		}
		if( NULL != ( pTask = TaskFactory( bCommandId, Message ) ).get() )
			vecRes.push_back( pTask );
	}
	return vecRes;
}

CTaskHandler::CTaskHandler()
{
	m_hCloseEv = CreateEvent( 0,0,0,0 );
	m_hCancelOpEv = CreateEvent( 0,1,0,0 );
	m_hProcessThread = (HANDLE)::_beginthreadex( 0, 0, fnProcessThread, this, 0, NULL );
	InitializeCriticalSection( &m_csTasks );
}

CTaskHandler::~CTaskHandler()
{
	SetEvent( m_hCloseEv );
	WaitForSingleObject( m_hProcessThread, 10000 );
	CloseHandle( m_hProcessThread );
	CloseHandle( m_hCloseEv );
	CloseHandle( m_hCancelOpEv );
	DeleteCriticalSection( &m_csTasks );
}
	
void CTaskHandler::AddTask( SmartPtr< CTask > pTask )
{
	EnterCriticalSection( &m_csTasks );
		if( !pTask->Immidiate() )
			m_deqTasks.push_back( pTask );
	LeaveCriticalSection( &m_csTasks );
}

unsigned _stdcall CTaskHandler::fnProcessThread( void* param )
{
	CTaskHandler* pThis = (CTaskHandler*)param;
		
	for(;;)
	{
		if( WAIT_OBJECT_0 == WaitForSingleObject( pThis->m_hCloseEv, 0 ) )
			break;
			
		SmartPtr< CTask > pTask;
		
		EnterCriticalSection( &pThis->m_csTasks );
			pTask = pThis->m_deqTasks.front();
			pThis->m_deqTasks.pop_front();
		LeaveCriticalSection( &pThis->m_csTasks );
		
		pTask->Execute();
	}
	return 0;
}

bool CGetStatus::Immidiate()
{
	m_csCurState.Enter();
		BYTE bResp[] = { RESP_OK, m_CurState }; 
		m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
	m_csCurState.Leave();
	return true;
}
	
bool CStartScan::Immidiate()
{
	BYTE bResp[] = { RESP_OK };
	m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
	return false;
}
	
bool CStopScan::Immidiate()
{
	m_csCurState.Enter();
		if( Scanning == m_CurState )
			Cancel();
	m_csCurState.Leave();
	BYTE bResp[] = { RESP_OK };	
	m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
	return true;
}

void CStartScan::Execute()
{
	m_vecData.clear();
	Log::instance().Trace( 90, "CAgent: �������� ������ �� ������ ������������" );
	m_csCurState.Enter();
		m_CurState = Scanning;	
	m_csCurState.Leave();

	for( std::vector< std::string >::iterator AddrIt = m_vecAddresses.begin(); AddrIt != m_vecAddresses.end() && WAIT_OBJECT_0 != WaitForSingleObject( m_CancelEv, 0 ); AddrIt++ )
	{
		for( PluginIterator PlugIt = m_PluginContainer.begin(); PlugIt != m_PluginContainer.end() && WAIT_OBJECT_0 != WaitForSingleObject( m_CancelEv, 0 ); PlugIt++ )
		{
			Log::instance().Trace( 80, "CAgent::StartScan: ��������� ����� %s � ������� ������� %s", AddrIt->c_str(), (*PlugIt)->GetProtocolName() );
			(*PlugIt)->Scan( *AddrIt, m_vecData, m_CancelEv );
		}
	}
	//TODO:��������,�������� ������� ������������ ������ �������� ����������,���� ���� ���������� 
	m_csCurState.Enter();
		m_CurState = Idling;		
	m_csCurState.Leave();
}

bool CGetData::Immidiate()
{
	Log::instance().Trace( 90, "CAgent: �������� ������ �� ��������� ������" );
	//������������ ������ ������ ��� ��������
	int iSize = 0;
	for( std::vector< std::string >::iterator It = m_vecData.begin(); It != m_vecData.end(); It++ )
	{
		//+1 - �� ����������� ����
		iSize += It->size() + 1;
	}
	//4 ����� �� ������ 1 ���� - ��������� ��������� �������
	iSize += 4 + 1; 
	std::auto_ptr< BYTE > pbBuf = std::auto_ptr< BYTE >( new BYTE[ iSize ] );
	pbBuf.get()[0] = (BYTE)RESP_OK;
	::memcpy( pbBuf .get() + 1, (void*)&iSize, 4 );
	int iOffset = 5; 
	for( std::vector< std::string >::iterator It = m_vecData.begin(); It != m_vecData.end(); It++ )
	{
		strcpy( (char*)pbBuf.get() + iOffset, It->c_str() );
		iOffset += It->size() + 1;
	}
	m_pSocket->Send( pbBuf.get(), iSize );
	return true;
}
