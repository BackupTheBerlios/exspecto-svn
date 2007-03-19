//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: �����, ��������������� �������������� � �������
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgentHandler.h"
#include <process.h>

CAgentHandler::CAgentHandler( std::string strAgentAddress ):m_strAddress( strAgentAddress )
														   ,m_bOpened( false )
														   ,m_bFinished( false )
{
	m_pConnectionHandler = SmartPtr< CConnectionHandler >( new CConnectionHandler( this ) );
}

CAgentHandler::~CAgentHandler()
{
	try
	{
		//����� ������������ ��������� ���������� � �������
		Close();
	}catch( ... ){}
}

//��������� ����� Msg ������ � �������� ����� � pbRespBuf, iRespSize - ��������� ������ ������
void CAgentHandler::SendMessage( CPacket &Msg, BYTE* pbRespBuf, int& iRespSize )throw( HandlerErr, CSocket::SocketErr )
{
	if( !m_bOpened )
	{
		Log::instance().Trace( 10, "CAgentHandler::SendMessage: ������!������ �� �������!" );
		throw HandlerErr( "Session has not been opened, but SendMessage called" );
	}
	BYTE* pBuf = NULL;
	int iSize = 0;
	//�������� ����� � ������� ��������� Msg ��� �������� �� ����
	Msg.GetBuffer( pBuf, iSize );
	Log::instance().Dump( 80, pBuf, iSize, "CAgentHandler::SendMessage: ���������� �����:" );
	if( 0 == iSize )
		throw HandlerErr( "Attempt to send message with zero-length was made" );		
	m_Sock.Send( pBuf, iSize );


	//�������� ����� �� ���������
	iRespSize = m_Sock.Receive( pbRespBuf, iRespSize );
	Log::instance().Dump( 80, pbRespBuf, iRespSize, "CAgentHandler::SendMessage: �������� �����:" );
	if( 0 == iRespSize )
		throw HandlerErr( "Connection closed" );
}

void CAgentHandler::Open()
{
	if( m_bOpened )
		return;
	try
	{
		Log::instance().Trace( 90, "CAgentHandler::Open: ��������" );
		m_Sock.Connect( m_strAddress, PORT );
		m_bOpened = true;
	}catch( CSocket::SocketErr& e )
	{
		Log::instance().Trace( 50, "CAgentHandler::Open: ������ ���������� � �������: %s; �������� ������: %s", m_strAddress.c_str(), e.what() );
		m_bOpened = false;
	}
}

void CAgentHandler::Close()
{
	try
	{
		Log::instance().Trace( 90, "CAgentHandler::Close: ��������" );
		m_Sock.Close();
		m_bOpened = false;
	}catch( CSocket::SocketErr& e )
	{
		Log::instance().Trace( 50, "CAgentHandler::Close: ������ �������� ���������� %s", e.what() );
		m_bOpened = false;
	}
}

bool CAgentHandler::IsOpened()const
{
	return m_bOpened;
}

void CAgentHandler::OnConnection( SmartPtr< CSocket > pSocket )
{
	Log::instance().Trace( 90, "CAgentHandler::OnConnection:.." );
	m_pConnectionHandler->Listen( pSocket );
}

void CAgentHandler::OnMessage( CPacket& Msg )
{
	BYTE bCommandId;
	Msg.GetCommandId( bCommandId );
	switch( bCommandId )
	{
		case ScanComplete:
			Log::instance().Trace( 90, "CAgentHandler::OnMessage: ������������ ���������" );
			m_bFinished = true;
			break;
	};
}

enumAgentResponse CAgentHandler::BeginScan( std::vector< std::string > vecAddresses )throw( HandlerErr, CSocket::SocketErr )
{
	Log::instance().Trace( 90, "CAgentHandler::BeginScan: �������� ������� ������ ������������" );
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( START_SCAN );
	Log::instance().Trace( 90, "CAgentHandler::BeginScan: ����� �������: %d", vecAddresses.size() );
	Msg.AddParam( (DWORD)vecAddresses.size() );
	for( std::vector< std::string >::iterator It = vecAddresses.begin(); It != vecAddresses.end(); It++ )
	{
		Log::instance().Trace( 92, "CAgentHandler::BeginScan: ��������� ����� %s", It->c_str() );
		Msg.AddAddress( *It );
	}
	Msg.EndCommand();
	
	
	Log::instance().Trace( 92, "CAgentHandler::BeginScan: ���������� ��������� ������: %s", m_strAddress.c_str() );
	SendMessage( Msg, pbRecvBuf, 1 );
	return (enumAgentResponse)pbRecvBuf[0]; 
}
	
enumAgentResponse CAgentHandler::StopScan()throw( HandlerErr, CSocket::SocketErr )
{
	Log::instance().Trace( 90, "CAgentHandler::StopScan: �������� ������� ��������� ������������" );
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( STOP_SCAN );
	Msg.EndCommand();

	SendMessage( Msg, pbRecvBuf, 1 );
	return (enumAgentResponse)pbRecvBuf[0];		
}
	
enumAgentResponse CAgentHandler::GetStatus( enumAgentState& Status )throw( HandlerErr, CSocket::SocketErr )
{
	Log::instance().Trace( 90, "CAgentHandler::GetStatus: �������� ������� ��������� �������" );
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( GET_STATUS );
	Msg.EndCommand();

	SendMessage( Msg, pbRecvBuf, 2 );
	if( RESP_OK != pbRecvBuf[0] )
	{
		Log::instance().Trace( 50, "CAgentHandler::GetStatus: ������� ��������� ������� �� ���������, ��� ��������: %d", pbRecvBuf[0] );
		return (enumAgentResponse)pbRecvBuf[0];
	}
	Status = (enumAgentState)pbRecvBuf[1];
	Log::instance().Trace( 80, "CAgentHandler::GetStatus: ������� ������: %d", Status );
	return RESP_OK;
}
	
enumAgentResponse CAgentHandler::GetData()throw( HandlerErr, CSocket::SocketErr )
{
	Log::instance().Trace( 90, "CAgentHandler::GetData: �������� ������� ��������� ������" );	
	CPacket Msg;
	BYTE pbRecvBuf[255];

	Msg.BeginCommand( GET_DATA );
	Msg.EndCommand();

	//���������� �������
	int iDataSize = 1;
	SendMessage( Msg, pbRecvBuf, iDataSize );
	Log::instance().Dump( 90, pbRecvBuf, iDataSize, "CAgentHandler::GetData: �������� ������:" );
	if( RESP_OK != pbRecvBuf[0] )
	{
		Log::instance().Trace( 50, "CAgentHandler::GetData: ������� ��������� ������ �� ���������, ��� ��������: %d", pbRecvBuf[0] );
		return (enumAgentResponse)pbRecvBuf[0];
	}
	SendMessage( Msg, pbRecvBuf, 4 );
	int iDataSize;
	::memcpy( (BYTE*)&iDataSize + 4, pbRecvBuf, 4 );
	Log::instance().Trace( 80, "CAgentHandler::GetData: ������ ������: %d", iDataSize );
	BYTE* pbData = new BYTE[ iDataSize ];
	//��������� ���, � ����� �������� ������
	SendMessage( Msg, pbData, iDataSize );
	Log::instance().Dump( 90, pbData, iDataSize, "CAgentHandler::GetData: �������� ������:" );
	delete pbData;
	return RESP_OK;
}

void CConnectionHandler::Listen( SmartPtr<CSocket> pSocket )
{
	m_pSocket = pSocket;
	if( WAIT_OBJECT_0 != WaitForSingleObject( m_hListenThread, 0 ) )
		m_hListenThread = (HANDLE)_beginthreadex( 0, 0, fnListenThread, this, 0, NULL );
}

unsigned __stdcall CConnectionHandler::fnListenThread( void* param )
{
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: ������" );
	CConnectionHandler* pThis = (CConnectionHandler*)param;
	CPacket Msg;
	BYTE pBuf[10240];
	int iCount;
	while( iCount = pThis->m_pSocket->Receive( pBuf, sizeof( pBuf ) ) )
	{
		Log::instance().Dump( 90, pBuf, iCount, "CConnectionHandler::fnListenThread: ������� �����:" );
		Msg.SetBuffer( pBuf, iCount );
		pThis->m_pAgentHandler->OnMessage( Msg );
	}
	Log::instance().Trace( 90, "CConnectionHandler::fnListenThread: ��������" );
	CloseHandle( pThis->m_hListenThread );
	return 0;
}

CConnectionHandler::~CConnectionHandler()
{
	SetEvent( m_hCloseEv );
	WaitForSingleObject( m_hListenThread, 10000 );
	CloseHandle( m_hCloseEv );
}
