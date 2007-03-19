#ifndef SERVERHANDLER_H_
#define SERVERHANDLER_H_

#include "ClientSocket.h"
#include "SmartPtr.hpp"
#include "Packet.h"
//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CServerHandler------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

class CServerHandler
{
public:

	CServerHandler( SmartPtr< CSocket > pMsgSocket, SmartPtr< CClientSocket > pEventSocket, const std::string& strServerAddress, int iEventPort );
	
	virtual ~CServerHandler();
	
	void SendEvent( CPacket& Event );
	
	void SendMsg( CPacket& Msg );
	
	void Receive( CPacket& Msg );
	
	std::string GetServerAddress();
	
	void CloseSession();

	CServerHandler( const CServerHandler& Handler )
	{
		*this = Handler;
	};
	
	CServerHandler& operator=( const CServerHandler& Handler )
	{
		m_pMsgSocket = Handler.m_pMsgSocket;
		m_pEventSocket = Handler.m_pEventSocket;
		m_strAddress = Handler.m_strAddress;
		m_iEventPort = Handler.m_iEventPort;
		return *this;
	};

	
private:
	
	SmartPtr< CSocket > m_pMsgSocket;
	
	SmartPtr< CClientSocket > m_pEventSocket;
	
	std::string m_strAddress;
	
	int m_iEventPort;
	
	static CCriticalSection m_csEventSocket;
	
};

#endif /*SERVERHANDLER_H_*/
