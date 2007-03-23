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
}
	
CServerHandler::~CServerHandler()
{
}
	
void CServerHandler::SendEvent( CPacket& Event )
{
	
	if( !m_pEventSocket->IsConnected() )
	{
		Log::instance().Trace( 99, "CServerHandler::SendEvent: Поднимаем канал для посылки события" );
		m_pEventSocket->Connect( m_strAddress, m_iEventPort );
	}
	BYTE *pbBuf;
	int iSize;
	Event.GetBuffer( pbBuf, iSize );
	m_pEventSocket->Send( pbBuf, iSize );
}
	
void CServerHandler::SendMsg( CPacket& Msg )
{
	BYTE *pbBuf;
	int iSize;
	Msg.GetBuffer( pbBuf, iSize );
	m_pMsgSocket->Send( pbBuf, iSize );
}

void CServerHandler::Receive( CPacket& Msg )
{
	BYTE pbBuf[10240];
	int iSize = 0;
	iSize = m_pMsgSocket->Receive( pbBuf, sizeof( pbBuf ) );
	Msg.Clear();
	Msg.SetBuffer( pbBuf, iSize );
}

std::string CServerHandler::GetServerAddress()
{
	if( m_pMsgSocket->IsConnected() )
		return m_pMsgSocket->GetRemoteHost().strAddr;
	else
		return "Соединение разорвано";
}

void CServerHandler::CloseSession()
{
	m_pMsgSocket->Close();
	m_pEventSocket->Close();
}
