//------------------------------------------------------------------------------------//
//Module: Agent handler class
//Author: Parshin Dmitry
//Description:
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CAgentHandler.h"
#include "DbProviderFactory.h"
#include <list>
#include <algorithm>



CAgentHandler::CAgentHandler( std::string strAgentAddress ):m_strAddress( strAgentAddress )
														   ,m_ScanFinished( false )
{
	m_pConnectionHandler = SmartPtr< CConnectionHandler >( new CConnectionHandler( this ) );
	m_vecRecvBuf.resize( RECEIVE_BUF_START_SIZE );
}

CAgentHandler::~CAgentHandler()
{
	try
	{
		Close();
	}catch( ... ){}
}

void CAgentHandler::SendMessage( COutPacket &Msg, CInPacket& Response )
{
	CLock lock( m_mtxExec );

	if( !IsOpened() )
	{
		Log::instance().Trace( 10, "CAgentHandler(%s)::SendMessage: Session has not been opened, but SendMessage called", m_strAddress.c_str() );
		throw HandlerErr( "Session has not been opened, but SendMessage called" );
	}
	Log::instance().Trace( 80, "CAgentHandler(%s)::SendMessage: Sending message: %s", m_strAddress.c_str(), Msg.ToString().c_str() );
	m_Sock << Msg;

	int iCount;
	bool bEnd = false;
	std::vector< BYTE > vecPacketBuf;

	while( !bEnd && ( iCount = m_Sock.Receive( &m_vecRecvBuf[0], (int)m_vecRecvBuf.size() ) ) > 0 )
	{
		Log::instance().Trace( 80, "CAgentHandler(%s)::SendMessage: iCount = %d", m_strAddress.c_str(), iCount );
		Log::instance().Dump( 200, (BYTE*)&m_vecRecvBuf[0], iCount, "CAgentHandler(%s)::SendMessage::Data received", m_strAddress.c_str() );
		if( ( iCount > (int)CInPacket::GetEndStamp().size() ) && ( 0 == memcmp( &CInPacket::GetEndStamp()[0], &m_vecRecvBuf[ iCount - CInPacket::GetEndStamp().size() ], CInPacket::GetEndStamp().size() ) ) )
		{
			iCount -= CInPacket::GetEndStamp().size();
			bEnd = true;
		}else if ( ( iCount == CInPacket::GetEndStamp().size() ) && ( 0 == memcmp( &CInPacket::GetEndStamp()[0], &m_vecRecvBuf[ 0 ], CInPacket::GetEndStamp().size() ) ) )
			break;
		
		vecPacketBuf.insert( vecPacketBuf.end(), m_vecRecvBuf.begin(), m_vecRecvBuf.begin() + iCount );
		
		if( (iCount == ( (int)m_vecRecvBuf.size() ) ) )
			if( (m_vecRecvBuf.size()<<1) <= RECEIVE_BUF_MAX_SIZE )
				m_vecRecvBuf.resize( m_vecRecvBuf.size()<<1 );
			else if( m_vecRecvBuf.size() < RECEIVE_BUF_MAX_SIZE )
				m_vecRecvBuf.resize( RECEIVE_BUF_MAX_SIZE );
		Log::instance().Trace( 80, "CAgentHandler(%s)::SendMessage: Receive buffer size: %d", m_strAddress.c_str(), m_vecRecvBuf.size() );
	}
	
	if( 0 == iCount )
		throw HandlerErr( "Connection closed" );
	Log::instance().Dump( 3, &vecPacketBuf[0], vecPacketBuf.size(), "CAgentHandler(%s)::SendMessage: Response :", m_strAddress.c_str() );
	Response.Load( &vecPacketBuf[0], vecPacketBuf.size() );
}

void CAgentHandler::Open()
{
	if( m_Sock.IsConnected() )
		return;
	try
	{
		int iAgentListenPort;
		Settings::instance().GetParam( AGENT_LISTEN_PORT, iAgentListenPort );
		Log::instance().Trace( 90, "CAgentHandler::Open: Connecting " );
		m_Sock.Connect( m_strAddress, iAgentListenPort );
	}catch( SocketErr& e )
	{
	  Log::instance().Trace( 50, "CAgentHandler(%s)::Open::Connection error: %s", m_strAddress.c_str(), e.what() );
	}
}

void CAgentHandler::Close()
{
	try
	{
		Log::instance().Trace( 90, "CAgentHandler(%s)::Close: ", m_strAddress.c_str() );
		if( !m_Sock.IsConnected() )
			return;
		m_Sock.Close();
	}catch( SocketErr& e )
	{
		Log::instance().Trace( 50, "CAgentHandler(%s)::Close: Closing error %s", m_strAddress.c_str(), e.what() );
	}
}

bool CAgentHandler::IsOpened()const
{
	return m_Sock.IsConnected();
}

void CAgentHandler::OnConnection( SmartPtr< CSocket > pSocket )
{
	Log::instance().Trace( 90, "CAgentHandler(%s)::OnConnection:..", m_strAddress.c_str() );
	m_pConnectionHandler->Listen( pSocket );
}

void CAgentHandler::OnMessage( CInPacket& Msg )
{
	try{
		std::string strEventId;
		Msg.GetField( EVENT_ID, strEventId );
		if( SCAN_COMPLETE == strEventId )
		{
			Log::instance().Trace( 90, "CAgentHandler(%s)::OnMessage:Getting data", m_strAddress.c_str() );
			GetData();
		}else
		  Log::instance().Trace( 3, "CAgentHandler(%s)::OnMessage:Event received: %s", m_strAddress.c_str(), strEventId.c_str() );
	}catch(std::exception& e)
	{
		Log::instance().Trace( 10, "CAgentHandler(%s)::OnMessage:Data receive error  %s", m_strAddress.c_str() , e.what() );
	}
	catch(...)
	{
		Log::instance().Trace( 10, "CAgentHandler(%s)::OnMessage: Unknown error ", m_strAddress.c_str() );
	}

}

std::string CAgentHandler::BeginScan( std::vector< std::string > vecAddresses )
{
	Log::instance().Trace( 90, "CAgentHandler(%s)::BeginScan: ", m_strAddress.c_str() );
	COutPacket Msg;

	Msg.PutField( COMMAND_ID, START_SCAN );
	Log::instance().Trace( 90, "CAgentHandler(%s)::BeginScan: addresses count: %d", m_strAddress.c_str(), vecAddresses.size() );
	Msg.PutField( ADDR_COUNT, (int)vecAddresses.size() );
	for( std::vector< std::string >::iterator It = vecAddresses.begin(); It != vecAddresses.end(); It++ )
	{
		Log::instance().Trace( 92, "CAgentHandler(%s)::BeginScan: Adding address %s", m_strAddress.c_str(), It->c_str() );
		Msg.PutField( HOST_ADDR, *It );
	}
	Log::instance().Trace( 92, "CAgentHandler(%s)::BeginScan: Sending comand", m_strAddress.c_str() );
	CInPacket Response;
	SendMessage( Msg, Response );
	std::string strStatus;
	Response.GetField( COMMAND_STAT, strStatus );
	m_ScanFinished.Reset();
	return strStatus; 
}
	
std::string CAgentHandler::StopScan()
{
	Log::instance().Trace( 90, "CAgentHandler(%s)::StopScan: ", m_strAddress.c_str() );
	COutPacket Msg;

	Msg.PutField( COMMAND_ID, STOP_SCAN );

	std::string strRes;
	CInPacket Response;
	SendMessage( Msg, Response );
	Response.GetField( COMMAND_STAT, strRes );
	return strRes; 
}
	
std::string CAgentHandler::GetStatus( std::string& Status )
{
	Log::instance().Trace( 90, "CAgentHandler(%s)::GetStatus: ", m_strAddress.c_str() );
	COutPacket Msg;
	Msg.PutField( COMMAND_ID, GET_STATUS );

	std::string strRes;
	CInPacket Response;
	SendMessage( Msg, Response );
	Response.GetField( COMMAND_STAT, strRes );
	if( AGENT_RESP_OK == strRes )
		Response.GetField( AGENT_STATUS, Status );
	return strRes;
}

std::string CAgentHandler::GetData()
{
	Log::instance().Trace( 90, "CAgentHandler::GetData(%s): ", m_strAddress.c_str() );	
	COutPacket Msg;

	Msg.PutField( COMMAND_ID, GET_DATA );
	Msg.PutField( FILES_COUNT, 1000 );

	std::string strRes;
	CInPacket Response;
	bool bFilesLeft = true;
	unsigned int uiTime = 0;
	while( bFilesLeft )
	{
		SendMessage( Msg, Response );
		Response.GetField( COMMAND_STAT, strRes );
		if( AGENT_RESP_OK != strRes )
			break;
		Response.GetField( FILES_LEFT, bFilesLeft );
		int iFilesCount;
		Response.GetField( FILES_COUNT, iFilesCount );
		if( iFilesCount > 0 )
		{
			std::set<std::string> setErased;
			hostRec TmpHost;
			Response.GetFirstHostRec( TmpHost );
			do
			{
				if( setErased.find( TmpHost.IPNum ) == setErased.end() )
				{
					//TODO: 
					DbProviderFactory::instance().GetProviderInstance()->EraseHost( TmpHost.HostName, TmpHost.IPNum, 0 );
					setErased.insert( TmpHost.IPNum );
				}
				DbProviderFactory::instance().GetProviderInstance()->AddFiles( TmpHost );
			}while( Response.GetNextHostRec( TmpHost ) );
		}
	}
	Log::instance().Trace( 0, "CAgentHandler::GetData(%s):", m_strAddress.c_str() );
	m_ScanFinished.Set();
	return strRes;
}

void CAgentHandler::CConnectionHandler::Listen( SmartPtr<CSocket> pSocket )
{
	m_pSocket = pSocket;
	if( !m_ListenThread.IsWorking() )
	  m_ListenThread.Start();
}

void CAgentHandler::CConnectionHandler::CListenThreadTask::Execute( CEvent& CancelEv  )
{
	Log::instance().Trace( 90, "CConnectionHandler(%s)::CListenThreadTask: ", m_pConnectionHandler->m_pAgentHandler->m_strAddress.c_str() );
	try{
		BYTE pBuf[10240];
		int iCount;
		while( iCount = m_pConnectionHandler->m_pSocket->Receive( pBuf, sizeof( pBuf ) ) )
		{
			Log::instance().Dump( 90, pBuf, iCount, "CConnectionHandler(%s)::fnListenThread: :", m_pConnectionHandler->m_pAgentHandler->m_strAddress.c_str() );
			CInPacket Msg( pBuf, iCount );
			m_pConnectionHandler->m_pAgentHandler->OnMessage( Msg );
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 10," CConnectionHandler(%s)::fnListenThread: : %s", m_pConnectionHandler->m_pAgentHandler->m_strAddress.c_str(), e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10," CConnectionHandler(%s)::fnListenThread: ", m_pConnectionHandler->m_pAgentHandler->m_strAddress.c_str() );
	}
	Log::instance().Trace( 90, "CConnectionHandler(%s)::fnListenThread: ", m_pConnectionHandler->m_pAgentHandler->m_strAddress.c_str() );
}

CAgentHandler::CConnectionHandler::~CConnectionHandler()
{
	if( NULL != m_pSocket.get() )
		m_pSocket->Close();
	m_CloseEv.Set();
}
