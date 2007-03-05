//-------------------------------------------------------------------------------------
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� �������������� � �������� (����� ��� ������� � ������� �����)
//-------------------------------------------------------------------------------------
#include "Socket.h"

//�����������, iType - ��� ������,����� ���� SOCK_STREAM/SOCK_DGRAM
//			   bBlocking - ��� �������, �� ��������� - �����������
CSocket::CSocket( int iType, bool bBlocking )throw( SocketErr ):m_Socket( INVALID_SOCKET ),
											 					m_bBlocking( bBlocking ),
											 					m_iType( iType )
{
	WSADATA WSAData;
	 //������������� �������
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )
		throw SocketErr( WSAGetLastError() );

	//������� �����
	if( INVALID_SOCKET == ( m_Socket = ::socket( AF_INET, iType, 0 ) ) )
		throw SocketErr( WSAGetLastError() );

	//������������� ��� �������
	SetBlocking( bBlocking );
}

//�����������, s - ��������� �������� ::socket �����
//			   bBlocking - ��� �������, �� ��������� - �����������
CSocket::CSocket( SOCKET s, bool bBlocking )throw( SocketErr ):m_bBlocking( bBlocking )
{
	WSADATA WSAData;
	//������������� �������
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )
	{
		throw SocketErr( WSAGetLastError() );
	}

	m_Socket = s;
	int Size = sizeof(int);
	::getsockopt( m_Socket, SOL_SOCKET, SO_TYPE, (char*)&m_iType, &Size );
}

CSocket::~CSocket(void)
{
	try
	{
		Close();
	}catch( std::exception& e )
	{
		Log::instance().Trace( 50, "CSocket::~CSocket: �������� ����������: %s", e.what() );
	};
}

//����� �������� ������
void CSocket::Close( void )throw( SocketErr )
{
	//TODO:����������� ��� ���������� ��� ������ shutdown ���� ���������� �� ����������� ���� ���������
	if( INVALID_SOCKET != m_Socket )
	{
		if( SOCKET_ERROR == ::closesocket( m_Socket ) )
			throw SocketErr( WSAGetLastError() );
		m_Socket = INVALID_SOCKET;
	}
}

//�����, ��������������� ��� �������(true - �����������,false - ������������� )
void CSocket::SetBlocking( bool bIsBlocking )
{
	unsigned long l = 1;
	m_bBlocking = bIsBlocking;
	if( m_bBlocking )
	{
		l = 0;
		::ioctlsocket( m_Socket, FIONBIO, (unsigned long* )&l );
	}else
	{
		::ioctlsocket( m_Socket, FIONBIO, (unsigned long* )&l );
	}
}

//����� ������� ������,���������� SOCKET_ERROR ���� ���-�� ������������ ����
int CSocket::Send( void* pBuffer, int iSize )throw( SocketErr )
{
	int res;
	if( SOCKET_ERROR == ( res = ::send( m_Socket, (const char*)pBuffer, iSize, 0 ) ) )
		throw SocketErr( WSAGetLastError() );
	return res;
}

//����� �����,���������� SOCKET_ERROR ���� ���-�� ������������ ����
int CSocket::Receive( void* pBuffer, int iBufSize )throw( SocketErr )
{
	int res;
	if( SOCKET_ERROR == ( res = ::recv( m_Socket, (char*)pBuffer, iBufSize, 0 ) ) )
	{
		Log::instance().Trace( 10," Reveive " );
		int iLastError = WSAGetLastError();
		Log::instance().Trace( 10," Reveive %d", iLastError );
		if( WSAEMSGSIZE == iLastError )
			throw SocketRespSizeErr();
		else
			throw SocketErr( WSAGetLastError() );
	}
	return res;
}

//����� ��������� ����� ���������� �����
CSocket::structAddr CSocket::GetRemoteHost()
{
	structAddr res;
	sockaddr_in sAddr;
	int len = sizeof(sAddr);
	hostent* hn;
	
	if( SOCKET_ERROR == getpeername( m_Socket, (sockaddr*)&sAddr, &len ) )
			throw SocketErr( WSAGetLastError() );
	else
	{
		res.iPort = ::ntohs( sAddr.sin_port );
		res.strAddr = ::inet_ntoa( sAddr.sin_addr );
		if( NULL != ( hn = ::gethostbyaddr( (const char*)&sAddr.sin_addr.S_un.S_addr, sizeof( sAddr.sin_addr.S_un.S_addr ), m_iType ) ) )
			res.strName = hn->h_name;
		return res;
	}
}
	
//��� ������������� ������������� �������, ����� ���������� true,���� � �������� �����
//��������� ������ � ����� ����������� �������� Receive
//Timeout - ����� �������� (���),���� -1,����������� ��������
bool CSocket::IsReadyForRead( int iTimeout )throw( SocketErr )
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

//��� ������������� ������������� �������, ����� ���������� true,���� ����� ����� �
//������
//Timeout - ����� �������� (���),���� -1,����������� ��������
bool CSocket::IsReadyForWrite( int iTimeout )throw( SocketErr )
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

