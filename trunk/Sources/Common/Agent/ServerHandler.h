#ifndef SERVERHANDLER_H_
#define SERVERHANDLER_H_

#include "ClientSocket.h"
#include "SmartPtr.hpp"
#include "Packet.h"
#include <vector>
//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CServerHandler------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

class CServerHandler
{
public:

	CServerHandler( SmartPtr< CSocket > pMsgSocket, SmartPtr< CClientSocket > pEventSocket, const std::string& strServerAddress, int iEventPort );

	virtual ~CServerHandler();

	void SendEvent( COutPacket& Event );

	void SendMsg( COutPacket& Msg, bool bEnd = true );

	void Receive( CInPacket& Msg );

	std::string GetServerAddress();

	void CloseSession();

	CServerHandler( const CServerHandler& Handler )
	{
		*this = Handler;
	};

	CServerHandler& operator=( const CServerHandler& Handler )
	{
		if( this != &Handler )
		{
			m_pMsgSocket = Handler.m_pMsgSocket;
			m_pEventSocket = Handler.m_pEventSocket;
			m_strAddress = Handler.m_strAddress;
			m_iEventPort = Handler.m_iEventPort;
			m_vecRecvBuf.resize( RECEIVE_BUF_START_SIZE );
		}
		return *this;
	};


private:

	SmartPtr< CSocket > m_pMsgSocket;

	SmartPtr< CClientSocket > m_pEventSocket;

	std::string m_strAddress;

	int m_iEventPort;

	//Приемный буфер
	std::vector<BYTE> m_vecRecvBuf;

};

#endif /*SERVERHANDLER_H_*/
