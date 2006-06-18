#include "StdAfx.h"
#include ".\socket.h"
#include "winsock2.h"
#include <string>

CSocket::CSocket( int iType, bool bBlocking ):m_iLastError(0)
											 ,m_Socket( INVALID_SOCKET )
											 ,m_bBlocking( bBlocking )
{
	//������������� �������
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), NULL ) )
	{
		m_iLastError = ::WSAGetLastError();
	}else
	{
		m_iLastError = 0;
	}
	//������� �����
	if( INVALID_SOCKET == ( m_Socket = ::socket( AF_INET, iType, 0 ) ) )
	{
		m_iLastError = ::WSAGetLastError();
	}else
	{
		m_iLastError = 0;
	}
	//������������� ��� �������
	SetBlocking( bBlocking );
}

CSocket::~CSocket(void)
{
	if( INVALID_SOCKET != m_Socket )
        ::shutdown( m_Socket, SD_BOTH );
	WSACleanup();
}

int CSocket::GetLastError(void)
{
	return m_iLastError;
}

bool CSocket::IsAddr(std::string strName)
{
	//�������� ��������: �������� ��������� ����� ������� � �������� ��������������� �� � �����
	//���� ���������� � ����� �� ������ 4, �� �� ��,����� strName - �� ip �����
		std::string::size_type PrevIndex = 0,Index = 0;
	for( int i = 0; i < 4; i++ )
	{
		if( std::string::npos != ( Index = strName.find( "." ) ) )
		{
			if( 0 == ::atoi( strName.substr( PrevIndex, Index ).c_str() ) )
				break;
			PrevIndex = Index;
		}else
			break;
	}
	return false;
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
		::ioctlsocket( m_Socket, FIONBIO, (unsigned long* )&l );
	else
	{
		l = FALSE;
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