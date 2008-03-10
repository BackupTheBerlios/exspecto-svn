//-------------------------------------------------------------------------------------//
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: 
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "ClientSocket.h"
#include <signal.h>

CClientSocket::CClientSocket( int iType, bool bBlocking ):CSocket( iType, bBlocking )
{
}

CClientSocket::~CClientSocket(void)
{
}

void CClientSocket::Connect( std::string strAddr, int iPort, int iTimeout )
{
  if( INVALID_SOCKET == ( m_Socket = ::socket( AF_INET, m_iType, 0 ) ) )
	throw SocketErr( WSAGetLastError() );
  SetBlocking( false );

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

  // Trying to connect with timeout 
  fd_set myset; 
  struct timeval tv; 
  socklen_t lon;
  int valopt;

  int res = ::connect( m_Socket, (sockaddr*)&sAddr, sizeof( sAddr ) ); 
  if (res < 0)
	{ 
	  if (errno == EINPROGRESS) { 
		//selecting
		do 
		  { 
			tv.tv_sec = iTimeout/1000; 
			tv.tv_usec = (iTimeout%1000)*1000; 
			FD_ZERO( &myset ); 
			FD_SET( m_Socket, &myset ); 

			res = ::select( m_Socket + 1, NULL, &myset, NULL, &tv ); 

			if (res < 0 && errno != EINTR) 
			  throw SocketErr( WSAGetLastError() );
			else if (res > 0) 
			  { 
				// Socket selected for write 
				lon = sizeof(int); 
				if( ::getsockopt( m_Socket, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon ) < 0 ) 
				  throw SocketErr( WSAGetLastError() );              // Check the value returned... 
				if (valopt) 
				  throw SocketErr( ::strerror(valopt) );
				break; 
			  } 
			else 
			  throw SocketErr( "Connection timeout" );
		  } while (1); 
	  } 
	  else 
		throw SocketErr( WSAGetLastError() );
	} 

  if( m_bBlocking )
	SetBlocking( true );

  SetConnected( true );

}
