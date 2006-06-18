#include "StdAfx.h"
#include ".\serversocket.h"
#include <string>

CServerSocket::CServerSocket( int iType, bool bBlocking ):CSocket( iType, bBlocking )
{
}

CServerSocket::~CServerSocket(void)
{
}

int CServerSocket::Bind( int iPort, std::string strAddr )
{
	sockaddr_in sAddr;
	int res;
	hostent* hn;

	::ZeroMemory( &sAddr, sizeof( sAddr ) );
	sAddr.sin_family = AF_INET;
	//Adress	
	if( "Any" == strAddr )
        sAddr.sin_addr.S_un.S_addr =  htonl(INADDR_ANY);
	else if( IsAddr( strAddr ) )
	{
		sAddr.sin_addr.S_un.S_addr = ::inet_addr( strAddr.c_str() );
	}else
	{
		if( NULL == ( hn = ::gethostbyname( strAddr.c_str() ) ) )
		{
			m_iLastError = ::WSAGetLastError();
			return 0;
		}
		sAddr.sin_addr.S_un.S_addr = ::inet_addr( hn->h_addr_list[0] );
	}
	//Port
	sAddr.sin_port = htons( iPort );
    if ( SOCKET_ERROR == ( res = bind( m_Socket, (sockaddr* )&sAddr, sizeof ( sAddr ) ) ) )
	{
		m_iLastError = WSAGetLastError();
		return SOCKET_ERROR;
	}
	return res;
}