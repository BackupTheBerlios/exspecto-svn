#include "ServerHandler.h"

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CServerHandler------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

CServerHandler::CServerHandler( SmartPtr< CSocket > pMsgSocket, SmartPtr< CSocket > pEventSocket ):m_pMsgSocket( pMsgSocket )
																								  ,m_pEventSocket( pEventSocket )
{
}
	
CServerHandler::~CServerHandler()
{
}
	
void CServerHandler::SendEvent( CPacket& Event )
{
	
	//TODO: нужно ли поднимать канал
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
	return m_pMsgSocket->GetRemoteHost().strAddr;
}

void CServerHandler::CloseSession()
{
	m_pMsgSocket->Close();
	m_pEventSocket->Close();
}
