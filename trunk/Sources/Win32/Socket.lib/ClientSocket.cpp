#include "StdAfx.h"
#include ".\clientsocket.h"
#include "winsock2.h"

CClientSocket::CClientSocket( int iType, bool bBlocking ):CSocket( iType, bBlocking )
{
}

CClientSocket::~CClientSocket(void)
{
}

int CClientSocket::Connect( std::string strAddr, int iPort )
{
	sockaddr_in sAddr;
	hostent* hn;
	int Result;

	::ZeroMemory( &sAddr, sizeof( sAddr ) );
	sAddr.sin_family = AF_INET;

	if( INADDR_NONE != ::inet_addr( strAddr.c_str() ) )
		sAddr.sin_addr.S_un.S_addr = ::inet_addr( strAddr.c_str() );
	else
	{
		if( NULL == ( hn = ::gethostbyname( strAddr.c_str() ) ) )
		{
			m_iLastError = ::WSAGetLastError();
			return 0;
		}
		sAddr.sin_addr.S_un.S_addr = ::inet_addr( hn->h_addr_list[0] );
	}
	sAddr.sin_port = ::htons( iPort );
	//Выполняем соединение
	if( SOCKET_ERROR == ( Result = ::connect( m_Socket, (sockaddr*)&sAddr, sizeof( sAddr ) ) ) )
	{
		m_iLastError = ::WSAGetLastError();
		return Result;
	}
	return 0;
}
