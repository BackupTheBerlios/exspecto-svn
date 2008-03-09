//-------------------------------------------------------------------------------------//
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: 
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "ClientSocket.h"

CClientSocket::CClientSocket( int iType, bool bBlocking ):CSocket( iType, bBlocking )
{
}

CClientSocket::~CClientSocket(void)
{
}


void CClientSocket::Connect( std::string strAddr, int iPort )
{
	if( INVALID_SOCKET == ( m_Socket = ::socket( AF_INET, m_iType, 0 ) ) )
		throw SocketErr( WSAGetLastError() );
	SetBlocking( m_bBlocking );

	sockaddr_in sAddr;
	hostent* hn;

    memset( &sAddr, 0, sizeof( sAddr ) );
	sAddr.sin_family = AF_INET;

	if( INADDR_NONE != ::inet_addr( strAddr.c_str() ) )
		sAddr.sin_addr.s_addr = ::inet_addr( strAddr.c_str() );
	else
	{
		if( NULL == ( hn = ::gethostbyname( strAddr.c_str() ) ) )
		{
			int iLastError = ::WSAGetLastError();
			if( HOST_NOT_FOUND == iLastError )
				throw SocketDNSErr();
			else
				throw SocketErr( iLastError );
		}
		sAddr.sin_addr.s_addr = ::inet_addr( hn->h_addr_list[0] );
	}
	sAddr.sin_port = htons( iPort );

	if( -1 == ::connect( m_Socket, (sockaddr*)&sAddr, sizeof( sAddr ) ) )
		throw SocketErr( WSAGetLastError() );
	SetConnected( true );
}
