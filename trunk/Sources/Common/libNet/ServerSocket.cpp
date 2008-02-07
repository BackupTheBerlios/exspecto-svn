//-------------------------------------------------------------------------------------//
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: CServerSocket class
//Author: Parshin Dmitry
//Description: �������ۜ��М������, �������՜��М��ۜ��؜��ל�����������؜��� ������՜������Ҝ��՜������ݜ������ ������М��������� ������ޜ��ڜ��՜�����ޜ���
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "ServerSocket.h"

//�������ޜ��ݜ���������������ڜ�����ޜ���
//iType - ������؜��� ������ޜ��ڜ��՜������, ���Ҝ��ޜ��ל��ܜ��ޜ��֜��ݜ������ ���ל��ݜ��М�����՜��ݜ��؜��� - SOCK_STREAM ���� SOCK_DGRAM
//bBlocking - ���ќ��ۜ��ޜ��ڜ��؜���������������؜��� ���ۜ��؜��ќ��� ���ݜ��� ���ќ��ۜ��ޜ��ڜ��؜���������������؜��� ������ޜ��ڜ��՜���
CServerSocket::CServerSocket( int iType, bool bBlocking ):CSocket( iType, bBlocking )
{
}

//�������ޜ��ߜ��؜���������������؜��� ���ڜ��ޜ��ݜ���������������ڜ�����ޜ���
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

//���Ĝ�����ݜ��ڜ�����؜��� "���ߜ������؜��ڜ������՜��ߜ��ۜ��՜��ݜ��؜���" ������ޜ��ڜ��՜������ ���� ���М��Ԝ������՜������, ���ߜ��� ������ܜ��ޜ��ۜ�����М��ݜ��؜��� �������� ������М��ܜ��� ���Ҝ�����ќ��؜������М��՜��� ���М��Ԝ������՜��� ���� ���ߜ��ޜ�������
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

//���Ĝ�����ݜ��ڜ�����؜��� ���ݜ��М�����М��ۜ��� "���ߜ������ޜ�����ۜ��������؜��Ҝ��М��ݜ��؜���", iMaxConn - ���ܜ��М��ڜ�����؜��ܜ��М��ۜ�����ݜ��ޜ��� ���ڜ��ޜ���-���Ҝ��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜���,
//SOMAXCONN - ���ܜ��М��ڜ�����؜��ܜ��М��ۜ�����ݜ��ޜ��� ���ל��ݜ��М�����՜��ݜ��؜���
void CServerSocket::Listen( int iMaxConn )
{
	if( (int)INVALID_SOCKET == ::listen( m_Socket, iMaxConn ) )
		throw SocketErr( WSAGetLastError() );
}

//������������ ���Ҝ�����ל��ޜ��Ҝ��� accept, ������ޜ��ڜ��՜��� ���ќ��ۜ��ޜ��ڜ��؜���������՜��������� ���Ҝ��ߜ��ۜ��ޜ������ ���Ԝ��� ���ߜ��ޜ�����Ҝ��ۜ��՜��ݜ��؜��� ������؜��Ӝ��ݜ��М��ۜ��� ���� ���Ҝ�����ޜ��Ԝ��������՜���
//������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜��� . ���Ĝ�����ݜ��ڜ�����؜��� ���Ҝ��ޜ��ל��Ҝ������М�����М��՜���
//���ݜ��ޜ��Ҝ������ ������ޜ��ڜ��՜���, ���ڜ��ޜ�����ޜ���������� ���ќ�����Ԝ��՜��� ���؜�����ߜ��ޜ��ۜ�����ל��ޜ��Ҝ��М������������ ���Ԝ��ۜ��� ������Ҝ�����ל��� ���� ���ߜ������؜�����ޜ��՜��Ԝ��؜��ݜ��؜��Ҝ�����՜��ٜ������ ���ܜ��М�����؜��ݜ��ޜ���
//(������؜��������՜��ܜ��� ������ޜ��ל��Ԝ��М������ ���՜��Ӝ��� ������М��ܜ���, ���ߜ������� ���������ߜ��՜�����ݜ��ޜ��� ������ޜ��՜��Ԝ��؜��ݜ��՜��ݜ��؜���). �������ޜ��ۜ��՜��� ���ߜ��ޜ��Ԝ������ޜ��ќ��ݜ������ ���Ԝ��М��ݜ��ݜ������
//���� ���ߜ������؜�����ޜ��՜��Ԝ��؜��ݜ��؜��Ҝ�����՜��ٜ������ ���ܜ��М�����؜��ݜ��� accept ���Ҝ��ޜ��ל��Ҝ������М�����М��՜��� ���� ���ߜ��М������М��ܜ��՜���������� addr
//(������؜��� ���М��Ԝ������՜������, IP-���М��Ԝ������՜���, ���ߜ��ޜ�������).
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
		//����������ۜ��� ���ޜ��֜��؜��Ԝ��М��ݜ��؜��� ���ޜ�����ܜ��՜��ݜ��՜��ݜ��� - ���Ҝ��������ޜ��Ԝ��؜��� ���ќ��՜��� ���ޜ�����؜��ќ��ڜ���
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
