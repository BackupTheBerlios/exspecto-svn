#include "StdAfx.h"
#include ".\serversocket.h"
#include <string>

CServerSocket::CServerSocket( int iType, bool bBlocking ):CSocket( iType, bBlocking )
{
}

CServerSocket::CServerSocket( CServerSocket& S ):CSocket( S.m_iType, S.m_bBlocking )
{
	m_Socket = S.m_Socket;
	m_iType = S.m_iType;
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
	sAddr.sin_family = m_iType;
	//Adress	
	if( "Any" == strAddr )
        sAddr.sin_addr.S_un.S_addr =  htonl(INADDR_ANY);
	else if( INADDR_NONE != ::inet_addr( strAddr.c_str() ) )
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

int CServerSocket::Listen( int iMaxConn )
{
	int res;
	if( INVALID_SOCKET == ( res = ::listen( m_Socket, iMaxConn ) ) )
	{
		m_iLastError = ::WSAGetLastError();
	}
	return res;
}

CSocket* CServerSocket::Accept( structAddr& addr )
{
	SOCKET s;
	CSocket* sock;
	sockaddr_in sAddr;
	int len = sizeof(sAddr);
	hostent* hn;

	ZeroMemory (&sAddr, sizeof (sAddr));
	
	if( INVALID_SOCKET == ( s = ::accept( m_Socket, (sockaddr*)&sAddr, &len ) ) )
	{
		m_iLastError = ::WSAGetLastError();
		return (CSocket*)INVALID_SOCKET;
	}else
	{
		addr.iPort = ::ntohs( sAddr.sin_port );
		addr.strAddr = ::inet_ntoa( sAddr.sin_addr );
		if( NULL != ( hn = ::gethostbyaddr( (const char*)&sAddr.sin_addr.S_un.S_addr, sizeof( sAddr.sin_addr.S_un.S_addr ), m_iType ) ) )
		{
			addr.strName = hn->h_name;
		}
		sock = new CSocket( s, m_bBlocking );
	}
	return sock;
}