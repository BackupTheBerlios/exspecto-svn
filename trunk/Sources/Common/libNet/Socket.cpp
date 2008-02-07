//-------------------------------------------------------------------------------------
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: �������ۜ��М������, �������՜��М��ۜ��؜��ל�����������؜��� ���Ҝ��ל��М��؜��ܜ��ޜ��Ԝ��՜��ٜ��������Ҝ��؜��� ���� ������ޜ��ڜ��՜�����М��ܜ��� (���ޜ��ќ�����М��� ���Ԝ��ۜ��� ���ڜ��ۜ��؜��՜��ݜ������ ���� ������՜������Ҝ��՜������� ������М���������)
//-------------------------------------------------------------------------------------
#include "Socket.h"
#include "SmartPtr.hpp"

//�������ޜ��ݜ���������������ڜ�����ޜ���, iType - ������؜��� ������ޜ��ڜ��՜������,���ܜ��ޜ��֜��՜��� ���ќ��������� SOCK_STREAM/SOCK_DGRAM
//			   bBlocking - ������؜��� ���Ҝ�����ל��ޜ��Ҝ��ޜ���, ���ߜ��� ������ܜ��ޜ��ۜ�����М��ݜ��؜��� - ���ќ��ۜ��ޜ��ڜ��؜���������������؜���
CSocket::CSocket( int iType, bool bBlocking ):m_Socket( INVALID_SOCKET ),
											 					m_bBlocking( bBlocking ),
											 					m_iType( iType ),
											 					m_bConnected( false )
{
    #ifdef WIN32
	WSADATA WSAData;
	 //�������ݜ��؜�����؜��М��ۜ��؜��ל��М�����؜��� ������ޜ��ڜ��՜�����ޜ���
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )
		throw SocketErr( WSAGetLastError() );
    #endif
	//�������ޜ��ל��Ԝ��М��՜��� ������ޜ��ڜ��՜���
	if( INVALID_SOCKET == ( m_Socket = ::socket( AF_INET, iType, 0 ) ) )
		throw SocketErr( WSAGetLastError() );

	//���Ü��������М��ݜ��М��Ҝ��ۜ��؜��Ҝ��М��՜��� ������؜��� ���Ҝ�����ל��ޜ��Ҝ��ޜ���
	SetBlocking( bBlocking );
}

//�������ޜ��ݜ���������������ڜ�����ޜ���, s - ������ޜ��ל��Ԝ��М��ݜ��ݜ������ ���������ݜ��ڜ�����؜��՜��� ::socket ������ޜ��ڜ��՜���
//			   bBlocking - ������؜��� ���Ҝ�����ל��ޜ��Ҝ��ޜ���, ���ߜ��� ������ܜ��ޜ��ۜ�����М��ݜ��؜��� - ���ќ��ۜ��ޜ��ڜ��؜���������������؜���
CSocket::CSocket( SOCKET s, bool bBlocking, bool bConnected ):m_bBlocking( bBlocking )
{
    #ifdef WIN32
	WSADATA WSAData;
	//�������ݜ��؜�����؜��М��ۜ��؜��ל��М�����؜��� ������ޜ��ڜ��՜�����ޜ���
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
		Log::instance().Trace( 50, "CSocket::~CSocket: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���: %s", e.what() );
	};
	#ifdef WIN32
	WSACleanup();
	#endif
}

//�������՜�����ޜ��� ���ל��М��ڜ������������؜��� ������ޜ��ڜ��՜������
void CSocket::Close( void )
{
	//TODO:�������М��ל��ޜ��ќ������М������������ ���������� ���ߜ������ޜ��؜��������ޜ��Ԝ��؜��� ���ߜ������� ���Ҝ�����ל��ޜ��Ҝ��� shutdown ���՜�����ۜ��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� ���ݜ��� ������������М��ݜ��ޜ��Ҝ��ۜ��՜��ݜ��� ���ۜ��؜��ќ��� �������М��ל��ޜ������Ҝ��М��ݜ���
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

//�������՜�����ޜ���, ������������М��ݜ��М��Ҝ��ۜ��؜��Ҝ��М��������؜��� ������؜��� ���Ҝ�����ל��ޜ��Ҝ��ޜ���(true - ���ќ��ۜ��ޜ��ڜ��؜���������������؜���,false - ���ݜ��՜��ќ��ۜ��ޜ��ڜ��؜���������������؜��� )
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

//�������՜�����ޜ��� ���ߜ��ޜ��������ۜ��ڜ��� ���Ԝ��М��ݜ��ݜ������,���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� SOCKET_ERROR ���ۜ��؜��ќ��� ���ڜ��ޜ���-���Ҝ��� ���ޜ�����ߜ������М��Ҝ��ۜ��՜��ݜ��ݜ������ ���ќ��М��ٜ���
void CSocket::Send( void* pBuffer, int iSize )
{
	int res;
	if( SOCKET_ERROR == ( res = ::send( m_Socket, (const char*)pBuffer, iSize, 0 ) ) )
		throw SocketErr( WSAGetLastError() );
	if( res != iSize )
	{
		Log::instance().Trace( 1, "CSocket::Send: �������ޜ��ۜ��؜�����՜��������Ҝ��� ���ߜ��ޜ�����ۜ��М��ݜ��ݜ������ ���� ������М��ڜ�����؜�����՜�����ڜ��� ���ޜ�����ߜ������М��Ҝ��ۜ��՜��ݜ��ݜ������ ���ќ��М��ٜ��� ���ݜ��� ������ޜ��Ҝ��ߜ��М��Ԝ��М��՜���" );
		throw SocketErr( "�������ޜ��ۜ��؜�����՜��������Ҝ��� ���ߜ��ޜ�����ۜ��М��ݜ��ݜ������ ���� ������М��ڜ�����؜�����՜�����ڜ��� ���ޜ�����ߜ������М��Ҝ��ۜ��՜��ݜ��ݜ������ ���ќ��М��ٜ��� ���ݜ��� ������ޜ��Ҝ��ߜ��М��Ԝ��М��՜���" );
	}
	SetConnected( true );
}

//�������՜�����ޜ��� ���ߜ������؜�����ܜ���,���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� SOCKET_ERROR ���ۜ��؜��ќ��� ���ڜ��ޜ���-���Ҝ��� ���ߜ��ޜ��ۜ��������՜��ݜ��ݜ������ ���ќ��М��ٜ���
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

//�������՜�����ޜ��� ���Ҝ��ޜ��ל��Ҝ���������М��՜��� ���М��Ԝ������՜��� ������Ԝ��М��ۜ��՜��ݜ��ݜ��ޜ��Ӝ��� ������ޜ���������
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

//������������ ���؜�����ߜ��ޜ��ۜ�����ל��ޜ��Ҝ��М��ݜ��؜��� ���ݜ��՜��ќ��ۜ��ޜ��ڜ��؜���������������؜��� ���Ҝ�����ל��ޜ��Ҝ��ޜ���, ���ܜ��՜�����ޜ��� ���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� true,���՜�����ۜ��� ���� ���ߜ������؜��՜��ܜ��ݜ������ ���ќ��������՜���
//���ߜ��ޜ�����������ߜ��؜��ۜ��� ���Ԝ��М��ݜ��ݜ������ ���� ���ܜ��ޜ��֜��ݜ��� ���ߜ������ޜ��؜��ל��Ҝ��ޜ��Ԝ��؜������ ���ޜ��ߜ��՜������М�����؜��� Receive
//Timeout - ���Ҝ������՜��ܜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� (���ܜ��ڜ���),���՜�����ۜ��� -1,���ќ��՜�����ڜ��ޜ��ݜ��՜�����ݜ��ޜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜���
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

//������������ ���؜�����ߜ��ޜ��ۜ�����ל��ޜ��Ҝ��М��ݜ��؜��� ���ݜ��՜��ќ��ۜ��ޜ��ڜ��؜���������������؜��� ���Ҝ�����ל��ޜ��Ҝ��ޜ���, ���ܜ��՜�����ޜ��� ���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� true,���՜�����ۜ��� ������ޜ��ڜ��՜��� ���Ӝ��ޜ�����ޜ��� ����
//���ל��М��ߜ��؜������
//Timeout - ���Ҝ������՜��ܜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� (���ܜ��ڜ���),���՜�����ۜ��� -1,���ќ��՜�����ڜ��ޜ��ݜ��՜�����ݜ��ޜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜���
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

