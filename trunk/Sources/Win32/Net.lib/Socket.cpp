#pragma once
#include "StdAfx.h"
#include ".\socket.h"
#include "winsock2.h"
#include <string>

CSocket::CSocket( int iType, bool bBlocking ):m_iLastError(0)
											 ,m_Socket( INVALID_SOCKET )
											 ,m_bBlocking( bBlocking )
											 ,m_iType( iType )
{
	WSADATA WSAData;
	 //Инициализация сокетов
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )
	{
		m_iLastError = ::WSAGetLastError();
	}else
	{
		m_iLastError = 0;
	}
	//Создаем сокет
	if( INVALID_SOCKET == ( m_Socket = ::socket( AF_INET, iType, 0 ) ) )
	{
		m_iLastError = ::WSAGetLastError();
	}else
	{
		m_iLastError = 0;
	}
	//Устанавливаем тип вызовов
	SetBlocking( bBlocking );
}

CSocket::CSocket( SOCKET s, bool bBlocking ):m_bBlocking( bBlocking )
{
	//Инициализация сокетов
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), NULL ) )
	{
		m_iLastError = ::WSAGetLastError();
	}else
	{
		m_iLastError = 0;
	}
	m_Socket = s;
	int Size = sizeof(int);
	::getsockopt( m_Socket, SOL_SOCKET, SO_TYPE, (char*)&m_iType, &Size ); 
}

CSocket::~CSocket(void)
{
	if( INVALID_SOCKET != m_Socket )
        ::shutdown( m_Socket, SD_BOTH );
}

int CSocket::GetLastError(void)
{
	return m_iLastError;
}

int CSocket::Close( void )
{
	int iRes = 0;
	if( INVALID_SOCKET != m_Socket )
		iRes = ::shutdown( m_Socket, SD_BOTH );
	return iRes;
}

void CSocket::SetBlocking( bool bIsBlocking )
{
	BOOL l = TRUE;
	m_bBlocking = bIsBlocking;
	if( m_bBlocking )
	{
		l = FALSE;
		::ioctlsocket( m_Socket, FIONBIO, (unsigned long* )&l );
	}else
	{
		::ioctlsocket( m_Socket, FIONBIO, (unsigned long* )&l );
	}
}

int CSocket::Send( void* pBuffer, int iSize )
{
	int res;
	if( SOCKET_ERROR == ( res = ::send( m_Socket, (const char*)pBuffer, iSize, 0 ) ) )
	{
		m_iLastError = WSAGetLastError();
	}
	return res;
}

int CSocket::Receive( void* pBuffer, int iBufSize )
{
	int res;
	if( SOCKET_ERROR == ( res = ::recv( m_Socket, (char*)pBuffer, iBufSize, 0 ) ) )
	{
		m_iLastError = WSAGetLastError();
	}
	return res;
}

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
	{
		m_iLastError = ::WSAGetLastError();
		return false;
	}
	if( FD_ISSET( m_Socket, &sReadSet ) )
	{
		return true;
	}
	return false;
}

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
	{
		m_iLastError = ::WSAGetLastError();
		return false;
	}
	if( FD_ISSET( m_Socket, &sWriteSet ) )
	{
		return true;
	}
	return false;
}