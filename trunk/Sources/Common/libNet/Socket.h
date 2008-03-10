//-------------------------------------------------------------------------------------
//Module: CSocket class
//Author: Parshin Dmitry
//Description:
//-------------------------------------------------------------------------------------

#ifndef CSOCKET_H_
#define CSOCKET_H_

#include "Singleton.hpp"
#include <stdexcept>
#include "CLog.h"
#ifdef WIN32
#include "winsock2.h"
#define socklen_t int
#define ioctl ioctlsocket
#else
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define WSAEMSGSIZE EMSGSIZE
#define WSAEINTR EINTR
#define WSAHOST_NOT_FOUND HOST_NOT_FOUND
typedef int SOCKET;
#endif
#include "Os_Spec.h"


class SocketErr: public std::exception
{
public:
	SocketErr( int iLastError )
	{
	    #ifdef WIN32
		if( 0 == FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iLastError, 0, data, sizeof(data), NULL ) )
		#else
		if( 0 == strerror_r( errno, data, sizeof( data ) ) )
		#endif
		{
			strcpy( data, "Error during formating error message" );
			#ifdef WIN32
			Log::instance().Trace( 0, "SocketErr: : %d", GetLastError() );
			#else
			Log::instance().Trace( 0, "SocketErr: errno = : %d", errno );
			#endif
		}
	};

	SocketErr( const std::string& Msg )
	{
		strcpy( data, Msg.c_str() );
	};

	virtual ~SocketErr()throw(){};

	virtual const char* what() const throw()
	{
		return data;
	};

private:

	char data[1024];
};


class CSocket
{
public:


	class SocketDNSErr: public SocketErr
	{
		public:
        SocketDNSErr():SocketErr( "DNS Error" ){};
		virtual ~SocketDNSErr()throw(){};
	};

	class SocketRespSizeErr: public SocketErr
	{
		public:
			SocketRespSizeErr():SocketErr( "Response size error" ){};
		virtual ~SocketRespSizeErr()throw(){};
	};

	class SocketConnectionLost: public SocketErr
	{
		public:
			SocketConnectionLost():SocketErr( "Connection lost" ){};
		virtual ~SocketConnectionLost()throw(){};
	};

	struct structAddr
	{
		std::string strAddr;
		std::string strName;
		int iPort;
	};


	CSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	CSocket( SOCKET s, bool bBlocking, bool bConnected );

	virtual ~CSocket(void);

    void Close( void );

	void Send( void* pBuffer, int iSize );

	int Receive( void* pBuffer, int iBufSize );

	structAddr GetRemoteHost();

	void SetBlocking( bool bIsBlocking );

	bool IsConnected()const;

	bool IsReadyForRead( int iTimeout = -1 );

	bool IsReadyForWrite( int iTimeout = -1 );

	bool GetPendingDataSize( u_long& ulSize );

protected:

	void SetConnected( bool bConnected );

	SOCKET m_Socket;

	bool m_bBlocking;

	int m_iType;

private:

	CSocket( const CSocket& );
	CSocket& operator=( const CSocket& );

	bool m_bConnected;
};


#endif
