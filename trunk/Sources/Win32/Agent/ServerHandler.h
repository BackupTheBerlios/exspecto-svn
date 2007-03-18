#ifndef SERVERHANDLER_H_
#define SERVERHANDLER_H_

#include "Socket.h"
#include "SmartPtr.hpp"
#include "Packet.h"
//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CServerHandler------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

class CServerHandler
{
public:

	CServerHandler( SmartPtr< CSocket > pMsgSocket, SmartPtr< CSocket > pEventSocket );
	
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
		return *this;
	};

	
private:
	
	SmartPtr< CSocket > m_pMsgSocket, m_pEventSocket;
	
	static CCriticalSection m_csEventSocket;
	
};

#endif /*SERVERHANDLER_H_*/
