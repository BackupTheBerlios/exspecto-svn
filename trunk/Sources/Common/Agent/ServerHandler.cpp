#include "precomp.h"
#include "precomp.h"
#include "precomp.h"
#include "ServerHandler.h"

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CServerHandler------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CServerHandler::CServerHandler( SmartPtr< CSocket > pMsgSocket,
								SmartPtr< CClientSocket > pEventSocket,
								const std::string& strServerAddress,
								int iEventPort ):m_pMsgSocket( pMsgSocket )
												,m_pEventSocket( pEventSocket )
												,m_strAddress( strServerAddress )
												,m_iEventPort( iEventPort )
{
  m_vecRecvBuf.resize( RECEIVE_BUF_START_SIZE );
}

CServerHandler::~CServerHandler()
{
}

void CServerHandler::SendEvent( COutPacket& Event )
{
  if( !m_pEventSocket->IsConnected() )
	{
	  Log::instance().Trace( 99, "%s: connection closed, connecting", __FUNCTION__ );
	  m_pEventSocket->Connect( m_strAddress, m_iEventPort );
	}
  *m_pEventSocket << Event;
}

void CServerHandler::SendMsg( COutPacket& Msg, bool bEnd )
{
  Log::instance().Trace( 200, "%s: Message : %s", __FUNCTION__, Msg.ToString().c_str() );
  *m_pMsgSocket << Msg;
}

void CServerHandler::Receive( CInPacket& Msg )
{
  /*
	BYTE pbBuf[10240];
	int iSize = 0;
	iSize = m_pMsgSocket->Receive( pbBuf, sizeof( pbBuf ) );
	Msg.Load( pbBuf, iSize );
  */
  int iCount;
  bool bEnd = false;
  std::vector< BYTE > vecPacketBuf;

  while( !bEnd && ( iCount = m_pMsgSocket->Receive( &m_vecRecvBuf[0], (int)m_vecRecvBuf.size() ) ) > 0 )
	{
	  Log::instance().Dump( 200, (BYTE*)&m_vecRecvBuf[0], iCount, "%s: Data received:", __FUNCTION__ );

	  if( ( iCount > (int)CInPacket::GetEndStamp().size() ) && ( 0 == memcmp( &CInPacket::GetEndStamp()[0], &m_vecRecvBuf[ iCount - CInPacket::GetEndStamp().size() ], CInPacket::GetEndStamp().size() ) ) )
		{
		  iCount -= CInPacket::GetEndStamp().size();
		  bEnd = true;
		}
	  else if ( ( iCount == (int)CInPacket::GetEndStamp().size() ) && ( 0 == memcmp( &CInPacket::GetEndStamp()[0], &m_vecRecvBuf[ 0 ], (int)CInPacket::GetEndStamp().size() ) ) )
		break;

	  vecPacketBuf.insert( vecPacketBuf.end(), m_vecRecvBuf.begin(), m_vecRecvBuf.begin() + iCount );

	  if( (iCount == ( (int)m_vecRecvBuf.size() ) ) )
		if( (m_vecRecvBuf.size()<<1) <= RECEIVE_BUF_MAX_SIZE )
		  m_vecRecvBuf.resize( m_vecRecvBuf.size()<<1 );
		else if( m_vecRecvBuf.size() < RECEIVE_BUF_MAX_SIZE )
		  m_vecRecvBuf.resize( RECEIVE_BUF_MAX_SIZE );

	  Log::instance().Trace( 80, "%s: Receive buffer size: %d", __FUNCTION__, m_vecRecvBuf.size() );
	}
  if( 0 == iCount )
	//TODO:
	throw std::runtime_error( "Connection closed" );
  Msg.Load( &vecPacketBuf[0], vecPacketBuf.size() );
}

std::string CServerHandler::GetServerAddress()
{
  if( m_pMsgSocket->IsConnected() )
	return m_pMsgSocket->GetRemoteHost().strAddr;
  else
	return std::string();
}

void CServerHandler::CloseSession()
{
  m_pMsgSocket->Close();
  m_pEventSocket->Close();
}
