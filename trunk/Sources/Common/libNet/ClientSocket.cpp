//-------------------------------------------------------------------------------------//
//œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ Exspecto 2006œóõ½.
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: œóõ½œóõ½œóõ½œóõ½œóõ½, œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "ClientSocket.h"

//œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½
//iType - œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½, œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ - SOCK_STREAM œóõ½ SOCK_DGRAM
//bBlocking - œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½
CClientSocket::CClientSocket( int iType, bool bBlocking ):CSocket( iType, bBlocking )
{
}

CClientSocket::~CClientSocket(void)
{
}


//œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½ strAddr:iPort,œóõ½œóõ½œóõ½ sAddr - œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½,œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½ IP-œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½
//								 iPort - œóõ½œóõ½œóõ½œóõ½, œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½
void CClientSocket::Connect( std::string strAddr, int iPort )
{
	//œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ - œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½
	if( INVALID_SOCKET == ( m_Socket = ::socket( AF_INET, m_iType, 0 ) ) )
		throw SocketErr( WSAGetLastError() );
	//œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½
	SetBlocking( m_bBlocking );

	sockaddr_in sAddr;
	hostent* hn;

    memset( &sAddr, 0, sizeof( sAddr ) );
	sAddr.sin_family = AF_INET;

	if( INADDR_NONE != ::inet_addr( strAddr.c_str() ) )
		sAddr.sin_addr.s_addr = ::inet_addr( strAddr.c_str() );
	else
	{
		//œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½ œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ - œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ DNS
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
	//œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½ œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½œóõ½
	if( SOCKET_ERROR == ::connect( m_Socket, (sockaddr*)&sAddr, sizeof( sAddr ) ) )
		throw SocketErr( WSAGetLastError() );
	SetConnected( true );
}
