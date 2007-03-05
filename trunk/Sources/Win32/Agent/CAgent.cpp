 //------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.									  
//Module: CAgent class																  
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#include "CAgent.h"
#include <process.h>
#include "SmartPtr.hpp"

enumAgentState CTask::m_CurState = Idling;
CCriticalSection CTask::m_csCurState;
CEvent CTask::m_CancelEv(false);
std::vector< std::string > CTask::m_vecData;
Container< CScanner*, PluginLoadStrategy > CStartScan::m_PluginContainer;

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------CAgent------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

//�����������,strSchedulerAddress - ����� ������������
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
				pThis->m_vecConnections.push_back( SmartPtr< CConnectionHandler >( new CConnectionHandler( client_sock ) ) );
			}else
				Log::instance().Trace( 50, "CAgent::ListenThread: ������ ����� � ������: %s. ����������", adr.strAddr.c_str() );
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

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CConnectionHandler--------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CConnectionHandler::CConnectionHandler( SmartPtr< CSocket > pSocket ):m_pSocket( pSocket )
													 				 ,m_MessageParser( m_pSocket )
{
	m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThread, this, 0, NULL );
}	
					
CConnectionHandler::~CConnectionHandler()
{
	try{
		Log::instance().Trace( 90, "CConnectionHandler::~CConnectionHandler: �������� ���������� � %s", m_pSocket->GetRemoteHost().strAddr.c_str() );
		m_CloseEv.Set();
		m_pSocket->Close();
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
	BYTE pBuf[10240];
	int iCount;

	try{
		Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: ������ ������ �������� �������� ��������� c ������ %s", pThis->m_pSocket->GetRemoteHost().strAddr.c_str() );
		while( iCount = pThis->m_pSocket->Receive( pBuf, 10240 ) )
		{
			Log::instance().Dump( 90, pBuf, iCount, "CConnectionHandler::fnListenThread: ������� �����: " );
			//������ ������ ��� ������� ��������� ������
			Msg.SetBuffer( pBuf, iCount );
			std::vector< SmartPtr< CTask > > vecTasks = pThis->m_MessageParser.Parse( Msg );
			for( std::vector< SmartPtr< CTask > >::iterator It = vecTasks.begin(); It != vecTasks.end(); It++ )
				pThis->m_TaskHandler.AddTask( *It );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: �������� ����������: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CConnectionHandler::fnListenThread: �������� ����������� ����������" );
	}
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: �������� ������ �������� �������� ��������� � ������ %s", pThis->m_pSocket->GetRemoteHost().strAddr.c_str() );
	return 0;	
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CMessageParser------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
	
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
	Log::instance().Trace( 95, "CMessageParser::Parse: ��������� ��������� ������" );
	BYTE bCommandId;
	std::vector< SmartPtr< CTask > > vecRes;
	SmartPtr< CTask > pTask;
	for(;;)
	{
		if( !Message.IsDone() )	
			Message.GetCommandId( bCommandId );
		else
		{
			Log::instance().Trace( 95, "CMessageParser::Parse: ��������� ��������� ������ ���������.����� ������:%d", vecRes.size() ); 
			break;
		}
		if( NULL != ( pTask = TaskFactory( bCommandId, Message ) ).get() )
			vecRes.push_back( pTask );
	}
	return vecRes;
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CTaskHandler--------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CTaskHandler::CTaskHandler()
{
	m_hProcessThread = (HANDLE)::_beginthreadex( 0, 0, fnProcessThread, this, 0, NULL );
}

CTaskHandler::~CTaskHandler()
{
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: �������� ����������� �������" );
	m_CloseEv.Set();
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: �������� �������� ������ ���������" );
	WaitForSingleObject( m_hProcessThread, 10000 );
	CloseHandle( m_hProcessThread );
	Log::instance().Trace( 95, "CTaskHandler::~CTaskHandler: �������� ����������� ������" );
}
	
void CTaskHandler::AddTask( SmartPtr< CTask > pTask )
{
	Log::instance().Trace( 95, "CTaskHandler::AddTask: ���������� �������: %s", pTask->GetDescription().c_str() );
	m_csTasks.Enter();
		if( !pTask->Immidiate() )
		{
			Log::instance().Trace( 95, "CTaskHandler::AddTask: ������� ����������� � ������� ����������" );
			m_deqTasks.push_back( pTask );
		}
	m_csTasks.Leave();
}

unsigned _stdcall CTaskHandler::fnProcessThread( void* param )
{
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: ������ ������ ����������� ������" );
	CTaskHandler* pThis = (CTaskHandler*)param;
	SmartPtr< CTask > pTask;
	try{
		for(;;)
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( pThis->m_CloseEv, 0 ) )
				break;
			
			pThis->m_csTasks.Enter();
				if( pThis->m_deqTasks.size() != 0 )
				{
					pTask = pThis->m_deqTasks.front();
					pThis->m_deqTasks.pop_front();
				}
			pThis->m_csTasks.Leave();
			
			if( pTask.get() )
			{			
				Log::instance().Trace( 10," CTaskHandler::fnProcessThread: ���������� �������: %s", pTask->GetDescription().c_str() );
				pTask->Execute();
				pTask.Release();
			}
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������� ����������: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CTaskHandler::fnProcessThread: �������� ����������� ����������" );
	}	
	Log::instance().Trace( 95, "CTaskHandler::fnProcessThread: ���������� ������ ����������� ������" );
	return 0;
}

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CTask---------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

bool CGetStatus::Immidiate()
{
	m_csCurState.Enter();
		BYTE bResp[] = { RESP_OK, m_CurState }; 
		m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
		Log::instance().Dump( 90, bResp, sizeof( bResp ), "CGetStatus:Immidiate: ��������� �����:" );
	m_csCurState.Leave();
	return true;
}
	
bool CStartScan::Immidiate()
{
	BYTE bResp[] = { RESP_OK };
	m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
	Log::instance().Dump( 90, bResp, sizeof( bResp ), "CStartScan:Immidiate: ��������� �����:" );
	return false;
}
	
void CStartScan::Execute()
{
	m_vecData.clear();
	Log::instance().Trace( 90, "CStartScan: �������� ������ �� ������ ������������" );
	m_csCurState.Enter();
		m_CurState = Scanning;	
	m_csCurState.Leave();

	for( std::vector< std::string >::iterator AddrIt = m_vecAddresses.begin(); AddrIt != m_vecAddresses.end(); AddrIt++ )
	{
		for( PluginIterator PlugIt = m_PluginContainer.begin(); PlugIt != m_PluginContainer.end(); PlugIt++ )
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( m_CancelEv, 0 ) )
				break;
			Log::instance().Trace( 80, "CStartScan: ��������� ����� %s � ������� ������� %s", AddrIt->c_str(), (*PlugIt)->GetProtocolName() );
			(*PlugIt)->Scan( *AddrIt, m_vecData, m_CancelEv );
		}
		if( WAIT_OBJECT_0 == WaitForSingleObject( m_CancelEv, 0 ) )
		{
			Log::instance().Trace( 90, "CStartScan: ������������ ��������" );
			//���������� ������� ������
			m_CancelEv.Reset();
			break;
		}
	}
	//TODO:��������,�������� ������� ������������ ������ �������� ����������,���� ���� ���������� 
	m_csCurState.Enter();
		m_CurState = Idling;		
	m_csCurState.Leave();
}

bool CStopScan::Immidiate()
{
	Log::instance().Trace( 90, "CStopScan: �������� ������ �� ������ ������������" );
	m_csCurState.Enter();
		if( Scanning == m_CurState )
		{
			Log::instance().Trace( 90, "CStopScan: �������� ������� ������������" );
			Cancel();
		}else
			Log::instance().Trace( 90, "CStopScan: � ������ ������ �� ��������� � ��������� ������������" );
	m_csCurState.Leave();
	BYTE bResp[] = { RESP_OK };	
	m_pSocket->Send( (void*)&bResp, sizeof( bResp ) );
	Log::instance().Dump( 90, bResp, sizeof( bResp ), "CStopScan:Immidiate: ��������� �����:" );
	return true;
}

bool CGetData::Immidiate()
{
	Log::instance().Trace( 90, "CGetData: �������� ������ �� ��������� ������" );
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
	Log::instance().Dump( 90, pbBuf.get(), iSize, "CGetData:Immidiate: ��������� �����:" );
	return true;
}
