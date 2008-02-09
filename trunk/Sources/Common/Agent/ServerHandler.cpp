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
		Log::instance().Trace( 99, "CServerHandler::SendEvent: œô¨¿œô¨Þœô¨Ôœô¨Ýœô¨Øœô¨Üœô¨Ðœô¨Õœô¨Ü œô¨Úœô¨Ðœô¨Ýœô¨Ðœô¨Û œô¨Ôœô¨Ûœô¨ï œô¨ßœô¨Þœô¨áœô¨ëœô¨Ûœô¨Úœô¨Ø œô¨áœô¨Þœô¨Ñœô¨ëœô¨âœô¨Øœô¨ï" );
		m_pEventSocket->Connect( m_strAddress, m_iEventPort );
	}
	*m_pEventSocket << Event;
}

void CServerHandler::SendMsg( COutPacket& Msg, bool bEnd )
{
	Log::instance().Trace( 200, "CServerHandler::SendMsg: œô¨¾œô¨âœô¨ßœô¨àœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨Õœô¨Ü œô¨áœô¨Þœô¨Þœô¨Ñœô¨éœô¨Õœô¨Ýœô¨Øœô¨Õ: %s", Msg.ToString().c_str() );
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
	//œô¨¿œô¨Þœô¨Ûœô¨ãœô¨çœô¨Ðœô¨Õœô¨Ü œô¨Þœô¨âœô¨Òœô¨Õœô¨â œô¨Ýœô¨Ð œô¨áœô¨Þœô¨Þœô¨Ñœô¨éœô¨Õœô¨Ýœô¨Øœô¨Õ
	while( !bEnd && ( iCount = m_pMsgSocket->Receive( &m_vecRecvBuf[0], (int)m_vecRecvBuf.size() ) ) > 0 )
	{
		Log::instance().Dump( 200, (BYTE*)&m_vecRecvBuf[0], iCount, "%s: œô¨¿œô¨àœô¨Øœô¨Ýœô¨ïœô¨âœô¨ë œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨Õ:", __FUNCTION__ );
		//œô¨¿œô¨àœô¨Þœô¨Òœô¨Õœô¨àœô¨ïœô¨Õœô¨Ü œô¨Ýœô¨Ð œô¨Úœô¨Þœô¨Ýœô¨Õœô¨æ œô¨ßœô¨Ðœô¨Úœô¨Õœô¨âœô¨Ð
		if( ( iCount > (int)CInPacket::GetEndStamp().size() ) && ( 0 == memcmp( &CInPacket::GetEndStamp()[0], &m_vecRecvBuf[ iCount - CInPacket::GetEndStamp().size() ], CInPacket::GetEndStamp().size() ) ) )
		{
			iCount -= CInPacket::GetEndStamp().size();
			bEnd = true;
		//œô¨µœô¨áœô¨Ûœô¨Ø œô¨ßœô¨Þœô¨Ûœô¨ãœô¨çœô¨Øœô¨Ûœô¨Ø œô¨âœô¨Þœô¨Ûœô¨ìœô¨Úœô¨Þ œô¨Üœô¨Ðœô¨àœô¨Úœô¨Õœô¨à œô¨Úœô¨Þœô¨Ýœô¨æœô¨Ð
		}else if ( ( iCount == (int)CInPacket::GetEndStamp().size() ) && ( 0 == memcmp( &CInPacket::GetEndStamp()[0], &m_vecRecvBuf[ 0 ], (int)CInPacket::GetEndStamp().size() ) ) )
			break;

		vecPacketBuf.insert( vecPacketBuf.end(), m_vecRecvBuf.begin(), m_vecRecvBuf.begin() + iCount );

		//œô¨Ãœô¨Òœô¨Õœô¨Ûœô¨Øœô¨çœô¨Øœô¨Òœô¨Ðœô¨Õœô¨Ü œô¨àœô¨Ðœô¨×œô¨Üœô¨Õœô¨à œô¨Ñœô¨ãœô¨äœô¨Õœô¨àœô¨Ð œô¨ßœô¨àœô¨Ø œô¨Ýœô¨Õœô¨Þœô¨Ñœô¨åœô¨Þœô¨Ôœô¨Øœô¨Üœô¨Þœô¨áœô¨âœô¨Ø
		if( (iCount == ( (int)m_vecRecvBuf.size() ) ) )
			if( (m_vecRecvBuf.size()<<1) <= RECEIVE_BUF_MAX_SIZE )
				m_vecRecvBuf.resize( m_vecRecvBuf.size()<<1 );
			else if( m_vecRecvBuf.size() < RECEIVE_BUF_MAX_SIZE )
				m_vecRecvBuf.resize( RECEIVE_BUF_MAX_SIZE );
		Log::instance().Trace( 80, "%s: œô¨Àœô¨Ðœô¨×œô¨Üœô¨Õœô¨à œô¨ßœô¨àœô¨Øœô¨Õœô¨Üœô¨Ýœô¨Þœô¨Óœô¨Þ œô¨Ñœô¨ãœô¨äœô¨Õœô¨àœô¨Ð: %d", __FUNCTION__, m_vecRecvBuf.size() );
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
		return "œô¨Áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨àœô¨Ðœô¨×œô¨Þœô¨àœô¨Òœô¨Ðœô¨Ýœô¨Þ";
}

void CServerHandler::CloseSession()
{
	m_pMsgSocket->Close();
	m_pEventSocket->Close();
}
