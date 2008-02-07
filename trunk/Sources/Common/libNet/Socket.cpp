//-------------------------------------------------------------------------------------
//œô¨Íœô¨âœô¨Þœô¨â œô¨äœô¨Ðœô¨Ùœô¨Û œô¨ïœô¨Òœô¨Ûœô¨ïœô¨Õœô¨âœô¨áœô¨ï œô¨çœô¨Ðœô¨áœô¨âœô¨ìœô¨î œô¨ßœô¨àœô¨Þœô¨Õœô¨Úœô¨âœô¨Ð Exspecto 2006œô¨Ó.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: œô¨ºœô¨Ûœô¨Ðœô¨áœô¨á, œô¨àœô¨Õœô¨Ðœô¨Ûœô¨Øœô¨×œô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨Òœô¨×œô¨Ðœô¨Øœô¨Üœô¨Þœô¨Ôœô¨Õœô¨Ùœô¨áœô¨âœô¨Òœô¨Øœô¨Õ œô¨á œô¨áœô¨Þœô¨Úœô¨Õœô¨âœô¨Ðœô¨Üœô¨Ø (œô¨Þœô¨Ñœô¨éœô¨Ðœô¨ï œô¨Ôœô¨Ûœô¨ï œô¨Úœô¨Ûœô¨Øœô¨Õœô¨Ýœô¨âœô¨Ð œô¨Ø œô¨áœô¨Õœô¨àœô¨Òœô¨Õœô¨àœô¨Ð œô¨çœô¨Ðœô¨áœô¨âœô¨ì)
//-------------------------------------------------------------------------------------
#include "Socket.h"
#include "SmartPtr.hpp"

//œô¨ºœô¨Þœô¨Ýœô¨áœô¨âœô¨àœô¨ãœô¨Úœô¨âœô¨Þœô¨à, iType - œô¨âœô¨Øœô¨ß œô¨áœô¨Þœô¨Úœô¨Õœô¨âœô¨Ð,œô¨Üœô¨Þœô¨Öœô¨Õœô¨â œô¨Ñœô¨ëœô¨âœô¨ì SOCK_STREAM/SOCK_DGRAM
//			   bBlocking - œô¨âœô¨Øœô¨ß œô¨Òœô¨ëœô¨×œô¨Þœô¨Òœô¨Þœô¨Ò, œô¨ßœô¨Þ œô¨ãœô¨Üœô¨Þœô¨Ûœô¨çœô¨Ðœô¨Ýœô¨Øœô¨î - œô¨Ñœô¨Ûœô¨Þœô¨Úœô¨Øœô¨àœô¨ãœô¨îœô¨éœô¨Øœô¨Õ
CSocket::CSocket( int iType, bool bBlocking ):m_Socket( INVALID_SOCKET ),
											 					m_bBlocking( bBlocking ),
											 					m_iType( iType ),
											 					m_bConnected( false )
{
    #ifdef WIN32
	WSADATA WSAData;
	 //œô¨¸œô¨Ýœô¨Øœô¨æœô¨Øœô¨Ðœô¨Ûœô¨Øœô¨×œô¨Ðœô¨æœô¨Øœô¨ï œô¨áœô¨Þœô¨Úœô¨Õœô¨âœô¨Þœô¨Ò
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )
		throw SocketErr( WSAGetLastError() );
    #endif
	//œô¨Áœô¨Þœô¨×œô¨Ôœô¨Ðœô¨Õœô¨Ü œô¨áœô¨Þœô¨Úœô¨Õœô¨â
	if( INVALID_SOCKET == ( m_Socket = ::socket( AF_INET, iType, 0 ) ) )
		throw SocketErr( WSAGetLastError() );

	//œô¨Ãœô¨áœô¨âœô¨Ðœô¨Ýœô¨Ðœô¨Òœô¨Ûœô¨Øœô¨Òœô¨Ðœô¨Õœô¨Ü œô¨âœô¨Øœô¨ß œô¨Òœô¨ëœô¨×œô¨Þœô¨Òœô¨Þœô¨Ò
	SetBlocking( bBlocking );
}

//œô¨ºœô¨Þœô¨Ýœô¨áœô¨âœô¨àœô¨ãœô¨Úœô¨âœô¨Þœô¨à, s - œô¨áœô¨Þœô¨×œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨Ù œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨Õœô¨Ù ::socket œô¨áœô¨Þœô¨Úœô¨Õœô¨â
//			   bBlocking - œô¨âœô¨Øœô¨ß œô¨Òœô¨ëœô¨×œô¨Þœô¨Òœô¨Þœô¨Ò, œô¨ßœô¨Þ œô¨ãœô¨Üœô¨Þœô¨Ûœô¨çœô¨Ðœô¨Ýœô¨Øœô¨î - œô¨Ñœô¨Ûœô¨Þœô¨Úœô¨Øœô¨àœô¨ãœô¨îœô¨éœô¨Øœô¨Õ
CSocket::CSocket( SOCKET s, bool bBlocking, bool bConnected ):m_bBlocking( bBlocking )
{
    #ifdef WIN32
	WSADATA WSAData;
	//œô¨¸œô¨Ýœô¨Øœô¨æœô¨Øœô¨Ðœô¨Ûœô¨Øœô¨×œô¨Ðœô¨æœô¨Øœô¨ï œô¨áœô¨Þœô¨Úœô¨Õœô¨âœô¨Þœô¨Ò
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )
	{
		throw SocketErr( WSAGetLastError() );
	}
    #endif
	SetConnected( bConnected );
	m_Socket = s;
	int Size = sizeof(int);
	::getsockopt( m_Socket, SOL_SOCKET, SO_TYPE, (char*)&m_iType, (socklen_t*)&Size );
}

CSocket::~CSocket(void)
{
	try
	{
		Close();
	}catch( std::exception& e )
	{
		Log::instance().Trace( 50, "CSocket::~CSocket: œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ: %s", e.what() );
	};
	#ifdef WIN32
	WSACleanup();
	#endif
}

//œô¨¼œô¨Õœô¨âœô¨Þœô¨Ô œô¨×œô¨Ðœô¨Úœô¨àœô¨ëœô¨âœô¨Øœô¨ï œô¨áœô¨Þœô¨Úœô¨Õœô¨âœô¨Ð
void CSocket::Close( void )
{
	//TODO:œô¨àœô¨Ðœô¨×œô¨Þœô¨Ñœô¨àœô¨Ðœô¨âœô¨ìœô¨áœô¨ï œô¨çœô¨âœô¨Þ œô¨ßœô¨àœô¨Þœô¨Øœô¨áœô¨åœô¨Þœô¨Ôœô¨Øœô¨â œô¨ßœô¨àœô¨Ø œô¨Òœô¨ëœô¨×œô¨Þœô¨Òœô¨Õ shutdown œô¨Õœô¨áœô¨Ûœô¨Ø œô¨áœô¨Þœô¨Õœô¨Ôœô¨Øœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Ýœô¨Õ œô¨ãœô¨áœô¨âœô¨Ðœô¨Ýœô¨Þœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨Þ œô¨Ûœô¨Øœô¨Ñœô¨Þ œô¨àœô¨Ðœô¨×œô¨Þœô¨àœô¨Òœô¨Ðœô¨Ýœô¨Þ
	if( INVALID_SOCKET != m_Socket )
	{
	    #ifdef WIN32
		if( SOCKET_ERROR == ::closesocket( m_Socket ) )
			throw SocketErr( WSAGetLastError() );
        #else
        close( m_Socket );
        #endif
		m_Socket = INVALID_SOCKET;
		SetConnected( false );
	}
}

//œô¨¼œô¨Õœô¨âœô¨Þœô¨Ô, œô¨ãœô¨áœô¨âœô¨Ðœô¨Ýœô¨Ðœô¨Òœô¨Ûœô¨Øœô¨Òœô¨Ðœô¨îœô¨éœô¨Øœô¨Ù œô¨âœô¨Øœô¨ß œô¨Òœô¨ëœô¨×œô¨Þœô¨Òœô¨Þœô¨Ò(true - œô¨Ñœô¨Ûœô¨Þœô¨Úœô¨Øœô¨àœô¨ãœô¨îœô¨éœô¨Øœô¨Õ,false - œô¨Ýœô¨Õœô¨Ñœô¨Ûœô¨Þœô¨Úœô¨Øœô¨àœô¨ãœô¨îœô¨éœô¨Øœô¨Õ )
void CSocket::SetBlocking( bool bIsBlocking )
{
	unsigned long l = 1;
	m_bBlocking = bIsBlocking;
	if( m_bBlocking )
	{
		l = 0;
	}
    ::ioctl( m_Socket, FIONBIO, (unsigned long* )&l );
}

//œô¨¼œô¨Õœô¨âœô¨Þœô¨Ô œô¨ßœô¨Þœô¨áœô¨ëœô¨Ûœô¨Úœô¨Ø œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å,œô¨Òœô¨Þœô¨×œô¨Òœô¨àœô¨Ðœô¨éœô¨Ðœô¨Õœô¨â SOCKET_ERROR œô¨Ûœô¨Øœô¨Ñœô¨Þ œô¨Úœô¨Þœô¨Û-œô¨Òœô¨Þ œô¨Þœô¨âœô¨ßœô¨àœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨Ýœô¨ëœô¨å œô¨Ñœô¨Ðœô¨Ùœô¨â
void CSocket::Send( void* pBuffer, int iSize )
{
	int res;
	if( SOCKET_ERROR == ( res = ::send( m_Socket, (const char*)pBuffer, iSize, 0 ) ) )
		throw SocketErr( WSAGetLastError() );
	if( res != iSize )
	{
		Log::instance().Trace( 1, "CSocket::Send: œô¨ºœô¨Þœô¨Ûœô¨Øœô¨çœô¨Õœô¨áœô¨âœô¨Òœô¨Þ œô¨ßœô¨Þœô¨áœô¨Ûœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å œô¨Ø œô¨äœô¨Ðœô¨Úœô¨âœô¨Øœô¨çœô¨Õœô¨áœô¨Úœô¨Ø œô¨Þœô¨âœô¨ßœô¨àœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨Ýœô¨ëœô¨å œô¨Ñœô¨Ðœô¨Ùœô¨â œô¨Ýœô¨Õ œô¨áœô¨Þœô¨Òœô¨ßœô¨Ðœô¨Ôœô¨Ðœô¨Õœô¨â" );
		throw SocketErr( "œô¨ºœô¨Þœô¨Ûœô¨Øœô¨çœô¨Õœô¨áœô¨âœô¨Òœô¨Þ œô¨ßœô¨Þœô¨áœô¨Ûœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å œô¨Ø œô¨äœô¨Ðœô¨Úœô¨âœô¨Øœô¨çœô¨Õœô¨áœô¨Úœô¨Ø œô¨Þœô¨âœô¨ßœô¨àœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨Ýœô¨ëœô¨å œô¨Ñœô¨Ðœô¨Ùœô¨â œô¨Ýœô¨Õ œô¨áœô¨Þœô¨Òœô¨ßœô¨Ðœô¨Ôœô¨Ðœô¨Õœô¨â" );
	}
	SetConnected( true );
}

//œô¨¼œô¨Õœô¨âœô¨Þœô¨Ô œô¨ßœô¨àœô¨Øœô¨ñœô¨Üœô¨Ð,œô¨Òœô¨Þœô¨×œô¨Òœô¨àœô¨Ðœô¨éœô¨Ðœô¨Õœô¨â SOCKET_ERROR œô¨Ûœô¨Øœô¨Ñœô¨Þ œô¨Úœô¨Þœô¨Û-œô¨Òœô¨Þ œô¨ßœô¨Þœô¨Ûœô¨ãœô¨çœô¨Õœô¨Ýœô¨Ýœô¨ëœô¨å œô¨Ñœô¨Ðœô¨Ùœô¨â
int CSocket::Receive( void* pBuffer, int iBufSize )
{
	int res;
	if( SOCKET_ERROR == ( res = ::recv( m_Socket, (char*)pBuffer, iBufSize, 0 ) ) )
	{
		int iLastError = WSAGetLastError();
		if( WSAEMSGSIZE == iLastError )
		{
			throw SocketRespSizeErr();
		}else
		{
			SocketErr s( iLastError );
			throw s;
		}
	}else if( 0 == res )
	{
		SetConnected( false );
		throw SocketConnectionLost();
	}
	SetConnected( true );
	return res;
}

//œô¨¼œô¨Õœô¨âœô¨Þœô¨Ô œô¨Òœô¨Þœô¨×œô¨Òœô¨àœô¨éœô¨Ðœô¨Õœô¨â œô¨Ðœô¨Ôœô¨àœô¨Õœô¨á œô¨ãœô¨Ôœô¨Ðœô¨Ûœô¨Õœô¨Ýœô¨Ýœô¨Þœô¨Óœô¨Þ œô¨åœô¨Þœô¨áœô¨âœô¨Ð
CSocket::structAddr CSocket::GetRemoteHost()
{
	structAddr res;
	sockaddr_in sAddr;
	int len = sizeof(sAddr);
	hostent* hn;

	if( SOCKET_ERROR == getpeername( m_Socket, (sockaddr*)&sAddr, (socklen_t*)&len ) )
		throw SocketErr( WSAGetLastError() );
	else
	{
		SetConnected( true );
		res.iPort = ntohs( sAddr.sin_port );
		res.strAddr = ::inet_ntoa( sAddr.sin_addr );
		if( NULL != ( hn = ::gethostbyaddr( (const char*)&sAddr.sin_addr.s_addr, sizeof( sAddr.sin_addr.s_addr ), m_iType ) ) )
			res.strName = hn->h_name;
		return res;
	}
}

//œô¨¿œô¨àœô¨Ø œô¨Øœô¨áœô¨ßœô¨Þœô¨Ûœô¨ìœô¨×œô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Ø œô¨Ýœô¨Õœô¨Ñœô¨Ûœô¨Þœô¨Úœô¨Øœô¨àœô¨ãœô¨îœô¨éœô¨Øœô¨å œô¨Òœô¨ëœô¨×œô¨Þœô¨Òœô¨Þœô¨Ò, œô¨Üœô¨Õœô¨âœô¨Þœô¨Ô œô¨Òœô¨Þœô¨×œô¨Òœô¨àœô¨Ðœô¨éœô¨Ðœô¨Õœô¨â true,œô¨Õœô¨áœô¨Ûœô¨Ø œô¨Ò œô¨ßœô¨àœô¨Øœô¨Õœô¨Üœô¨Ýœô¨ëœô¨Ù œô¨Ñœô¨ãœô¨äœô¨Õœô¨à
//œô¨ßœô¨Þœô¨áœô¨âœô¨ãœô¨ßœô¨Øœô¨Ûœô¨Ø œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨Õ œô¨Ø œô¨Üœô¨Þœô¨Öœô¨Ýœô¨Þ œô¨ßœô¨àœô¨Þœô¨Øœô¨×œô¨Òœô¨Þœô¨Ôœô¨Øœô¨âœô¨ì œô¨Þœô¨ßœô¨Õœô¨àœô¨Ðœô¨æœô¨Øœô¨î Receive
//Timeout - œô¨Òœô¨àœô¨Õœô¨Üœô¨ï œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï (œô¨Üœô¨Úœô¨á),œô¨Õœô¨áœô¨Ûœô¨Ø -1,œô¨Ñœô¨Õœô¨áœô¨Úœô¨Þœô¨Ýœô¨Õœô¨çœô¨Ýœô¨Þœô¨Õ œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ
bool CSocket::IsReadyForRead( int iTimeout )
{
	fd_set sReadSet;
	timeval sTimeout;
	timeval* psTimeout = NULL;

	FD_ZERO( &sReadSet );
	FD_SET( m_Socket, &sReadSet );
	if( -1 != iTimeout )
	{
		sTimeout.tv_sec = 0;
		sTimeout.tv_usec = iTimeout;
		psTimeout = &sTimeout;
	}
	if( SOCKET_ERROR == ::select( 0, &sReadSet, NULL, NULL, psTimeout ) )
		throw SocketErr( WSAGetLastError() );
	if( FD_ISSET( m_Socket, &sReadSet ) )
		return true;
	return false;
}

//œô¨¿œô¨àœô¨Ø œô¨Øœô¨áœô¨ßœô¨Þœô¨Ûœô¨ìœô¨×œô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Ø œô¨Ýœô¨Õœô¨Ñœô¨Ûœô¨Þœô¨Úœô¨Øœô¨àœô¨ãœô¨îœô¨éœô¨Øœô¨å œô¨Òœô¨ëœô¨×œô¨Þœô¨Òœô¨Þœô¨Ò, œô¨Üœô¨Õœô¨âœô¨Þœô¨Ô œô¨Òœô¨Þœô¨×œô¨Òœô¨àœô¨Ðœô¨éœô¨Ðœô¨Õœô¨â true,œô¨Õœô¨áœô¨Ûœô¨Ø œô¨áœô¨Þœô¨Úœô¨Õœô¨â œô¨Óœô¨Þœô¨âœô¨Þœô¨Ò œô¨Ú
//œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨Ø
//Timeout - œô¨Òœô¨àœô¨Õœô¨Üœô¨ï œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï (œô¨Üœô¨Úœô¨á),œô¨Õœô¨áœô¨Ûœô¨Ø -1,œô¨Ñœô¨Õœô¨áœô¨Úœô¨Þœô¨Ýœô¨Õœô¨çœô¨Ýœô¨Þœô¨Õ œô¨Þœô¨Öœô¨Øœô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨Õ
bool CSocket::IsReadyForWrite( int iTimeout )
{
	fd_set sWriteSet;
	timeval sTimeout;
	timeval* psTimeout = NULL;

	FD_ZERO( &sWriteSet );
	FD_SET( m_Socket, &sWriteSet );
	if( -1 != iTimeout )
	{
		sTimeout.tv_sec = 0;
		sTimeout.tv_usec = iTimeout;
		psTimeout = &sTimeout;
	}
	if( SOCKET_ERROR == ::select( 0, NULL, &sWriteSet, NULL, psTimeout ) )
		throw SocketErr( WSAGetLastError() );
	if( FD_ISSET( m_Socket, &sWriteSet ) )
		return true;
	return false;
}

bool CSocket::IsConnected()const
{
	Log::instance().Trace( 50, "CSocket::IsConnected %s", m_bConnected?"true":"false" );
	return m_bConnected;
}

void CSocket::SetConnected( bool bConnected )
{
	m_bConnected = bConnected;
}

bool CSocket::GetPendingDataSize( u_long& ulSize )
{
	if( ioctl( m_Socket, FIONREAD, &ulSize ) == -1 ) return false;
	return true;
}

