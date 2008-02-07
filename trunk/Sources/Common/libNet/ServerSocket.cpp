//-------------------------------------------------------------------------------------//
//œô¨Íœô¨âœô¨Þœô¨â œô¨äœô¨Ðœô¨Ùœô¨Û œô¨ïœô¨Òœô¨Ûœô¨ïœô¨Õœô¨âœô¨áœô¨ï œô¨çœô¨Ðœô¨áœô¨âœô¨ìœô¨î œô¨ßœô¨àœô¨Þœô¨Õœô¨Úœô¨âœô¨Ð Exspecto 2006œô¨Ó.
//Module: CServerSocket class
//Author: Parshin Dmitry
//Description: œô¨ºœô¨Ûœô¨Ðœô¨áœô¨á, œô¨àœô¨Õœô¨Ðœô¨Ûœô¨Øœô¨×œô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨áœô¨Õœô¨àœô¨Òœô¨Õœô¨àœô¨Ýœô¨ãœô¨î œô¨çœô¨Ðœô¨áœô¨âœô¨ì œô¨áœô¨Þœô¨Úœô¨Õœô¨âœô¨Þœô¨Ò
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "ServerSocket.h"

//œô¨ºœô¨Þœô¨Ýœô¨áœô¨âœô¨àœô¨ãœô¨Úœô¨âœô¨Þœô¨à
//iType - œô¨âœô¨Øœô¨ß œô¨áœô¨Þœô¨Úœô¨Õœô¨âœô¨Ð, œô¨Òœô¨Þœô¨×œô¨Üœô¨Þœô¨Öœô¨Ýœô¨ëœô¨Õ œô¨×œô¨Ýœô¨Ðœô¨çœô¨Õœô¨Ýœô¨Øœô¨ï - SOCK_STREAM œô¨Ø SOCK_DGRAM
//bBlocking - œô¨Ñœô¨Ûœô¨Þœô¨Úœô¨Øœô¨àœô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨Ûœô¨Øœô¨Ñœô¨Þ œô¨Ýœô¨Õ œô¨Ñœô¨Ûœô¨Þœô¨Úœô¨Øœô¨àœô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨áœô¨Þœô¨Úœô¨Õœô¨â
CServerSocket::CServerSocket( int iType, bool bBlocking ):CSocket( iType, bBlocking )
{
}

//œô¨ºœô¨Þœô¨ßœô¨Øœô¨àœô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨Úœô¨Þœô¨Ýœô¨áœô¨âœô¨àœô¨ãœô¨Úœô¨âœô¨Þœô¨à
CServerSocket::CServerSocket( const CServerSocket& S ):CSocket( S.m_iType, S.m_bBlocking )
{
	*this = S;
}

CServerSocket& CServerSocket::operator=( const CServerSocket& s )
{
	if( this != &s )
	{
		m_Socket = s.m_Socket;
		m_iType = s.m_iType;
	}
	return *this;
}

CServerSocket::~CServerSocket(void)
{
}

//œô¨Äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨ï "œô¨ßœô¨àœô¨Øœô¨Úœô¨àœô¨Õœô¨ßœô¨Ûœô¨Õœô¨Ýœô¨Øœô¨ï" œô¨áœô¨Þœô¨Úœô¨Õœô¨âœô¨Ð œô¨Ú œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨ã, œô¨ßœô¨Þ œô¨ãœô¨Üœô¨Þœô¨Ûœô¨çœô¨Ðœô¨Ýœô¨Øœô¨î œô¨¾œô¨Á œô¨áœô¨Ðœô¨Üœô¨Ð œô¨Òœô¨ëœô¨Ñœô¨Øœô¨àœô¨Ðœô¨Õœô¨â œô¨Ðœô¨Ôœô¨àœô¨Õœô¨á œô¨Ø œô¨ßœô¨Þœô¨àœô¨â
void CServerSocket::Bind( int iPort, std::string strAddr )
{
	sockaddr_in sAddr;
	hostent* hn;

    memset( &sAddr, 0, sizeof( sAddr ) );
	sAddr.sin_family = AF_INET;
	//Adress
	if( "Any" == strAddr )
        sAddr.sin_addr.s_addr =  htonl(INADDR_ANY);
	else if( INADDR_NONE != ::inet_addr( strAddr.c_str() ) )
	{
		sAddr.sin_addr.s_addr = ::inet_addr( strAddr.c_str() );
	}else
	{
		if( NULL == ( hn = ::gethostbyname( strAddr.c_str() ) ) )
		{
			int iLastError = ::WSAGetLastError();
			if( WSAHOST_NOT_FOUND == iLastError )
				throw SocketDNSErr();
			else
				throw SocketErr( iLastError );
		}
		sAddr.sin_addr.s_addr = ::inet_addr( hn->h_addr_list[0] );
	}
	//Port
	sAddr.sin_port = htons( iPort );
	if ( SOCKET_ERROR == bind( m_Socket, (sockaddr* )&sAddr, sizeof ( sAddr ) ) )
		throw SocketErr( WSAGetLastError() );
}

//œô¨Äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨ï œô¨Ýœô¨Ðœô¨çœô¨Ðœô¨Ûœô¨Ð "œô¨ßœô¨àœô¨Þœô¨áœô¨Ûœô¨ãœô¨èœô¨Øœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï", iMaxConn - œô¨Üœô¨Ðœô¨Úœô¨áœô¨Øœô¨Üœô¨Ðœô¨Ûœô¨ìœô¨Ýœô¨Þœô¨Õ œô¨Úœô¨Þœô¨Û-œô¨Òœô¨Þ œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨ï,
//SOMAXCONN - œô¨Üœô¨Ðœô¨Úœô¨áœô¨Øœô¨Üœô¨Ðœô¨Ûœô¨ìœô¨Ýœô¨Þœô¨Õ œô¨×œô¨Ýœô¨Ðœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ
void CServerSocket::Listen( int iMaxConn )
{
	if( (int)INVALID_SOCKET == ::listen( m_Socket, iMaxConn ) )
		throw SocketErr( WSAGetLastError() );
}

//œô¨¿œô¨àœô¨Ø œô¨Òœô¨ëœô¨×œô¨Þœô¨Òœô¨Õ accept, œô¨áœô¨Þœô¨Úœô¨Õœô¨â œô¨Ñœô¨Ûœô¨Þœô¨Úœô¨Øœô¨àœô¨ãœô¨Õœô¨âœô¨áœô¨ï œô¨Òœô¨ßœô¨Ûœô¨Þœô¨âœô¨ì œô¨Ôœô¨Þ œô¨ßœô¨Þœô¨ïœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨Øœô¨ï œô¨áœô¨Øœô¨Óœô¨Ýœô¨Ðœô¨Ûœô¨Ð œô¨Þ œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨éœô¨Õœô¨Ü
//œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Ø . œô¨Äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨ï œô¨Òœô¨Þœô¨×œô¨Òœô¨àœô¨Ðœô¨éœô¨Ðœô¨Õœô¨â
//œô¨Ýœô¨Þœô¨Òœô¨ëœô¨Ù œô¨áœô¨Þœô¨Úœô¨Õœô¨â, œô¨Úœô¨Þœô¨âœô¨Þœô¨àœô¨ëœô¨Ù œô¨Ñœô¨ãœô¨Ôœô¨Õœô¨â œô¨Øœô¨áœô¨ßœô¨Þœô¨Ûœô¨ìœô¨×œô¨Þœô¨Òœô¨Ðœô¨âœô¨ìœô¨áœô¨ï œô¨Ôœô¨Ûœô¨ï œô¨áœô¨Òœô¨ïœô¨×œô¨Ø œô¨á œô¨ßœô¨àœô¨Øœô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Øœô¨Òœô¨èœô¨Õœô¨Ùœô¨áœô¨ï œô¨Üœô¨Ðœô¨èœô¨Øœô¨Ýœô¨Þœô¨Ù
//(œô¨áœô¨Øœô¨áœô¨âœô¨Õœô¨Üœô¨Ð œô¨áœô¨Þœô¨×œô¨Ôœô¨Ðœô¨ñœô¨â œô¨Õœô¨Óœô¨Þ œô¨áœô¨Ðœô¨Üœô¨Ð, œô¨ßœô¨àœô¨Ø œô¨ãœô¨áœô¨ßœô¨Õœô¨èœô¨Ýœô¨Þœô¨Ü œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Ø). œô¨±œô¨Þœô¨Ûœô¨Õœô¨Õ œô¨ßœô¨Þœô¨Ôœô¨àœô¨Þœô¨Ñœô¨Ýœô¨ëœô¨Õ œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨Õ
//œô¨Þ œô¨ßœô¨àœô¨Øœô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Øœô¨Òœô¨èœô¨Õœô¨Ùœô¨áœô¨ï œô¨Üœô¨Ðœô¨èœô¨Øœô¨Ýœô¨Õ accept œô¨Òœô¨Þœô¨×œô¨Òœô¨àœô¨Ðœô¨éœô¨Ðœô¨Õœô¨â œô¨Ò œô¨ßœô¨Ðœô¨àœô¨Ðœô¨Üœô¨Õœô¨âœô¨àœô¨Õ addr
//(œô¨âœô¨Øœô¨ß œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨Ð, IP-œô¨Ðœô¨Ôœô¨àœô¨Õœô¨á, œô¨ßœô¨Þœô¨àœô¨â).
SmartPtr< CSocket > CServerSocket::Accept( structAddr& addr )
{
	SOCKET s;
	SmartPtr< CSocket > sock;
	sockaddr_in sAddr;
	int len = sizeof(sAddr);
	hostent* hn;
	memset( &sAddr, 0, sizeof( sAddr ) );
	SetConnected( true );
	if( INVALID_SOCKET == ( s = ::accept( m_Socket, (sockaddr*)&sAddr, (socklen_t*)&len ) ) )
	{
		SetConnected( false );
		int iLastErr;
		//œô¨µœô¨áœô¨Ûœô¨Ø œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨Þœô¨âœô¨Üœô¨Õœô¨Ýœô¨Õœô¨Ýœô¨Þ - œô¨Òœô¨ëœô¨åœô¨Þœô¨Ôœô¨Øœô¨Ü œô¨Ñœô¨Õœô¨× œô¨Þœô¨èœô¨Øœô¨Ñœô¨Úœô¨Ø
		if( WSAEINTR == ( iLastErr = WSAGetLastError() ) )
			return sock;
		throw SocketErr( WSAGetLastError() );
	}else
	{
		addr.iPort = ntohs( sAddr.sin_port );
		addr.strAddr = ::inet_ntoa( sAddr.sin_addr );
		if( NULL != ( hn = ::gethostbyaddr( (const char*)&sAddr.sin_addr.s_addr, sizeof( sAddr.sin_addr.s_addr ), m_iType ) ) )
		{
			addr.strName = hn->h_name;
		}
		sock = SmartPtr< CSocket >( new CSocket( s, m_bBlocking, true ) );
	}
	SetConnected( false );
	return sock;
}
