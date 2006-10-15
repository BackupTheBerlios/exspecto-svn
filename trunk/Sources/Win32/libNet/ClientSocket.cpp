//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� ���������� ����� �������
//-------------------------------------------------------------------------------------//
#include "clientsocket.h"

//�����������
//iType - ��� ������, ��������� �������� - SOCK_STREAM � SOCK_DGRAM
//bBlocking - ����������� ���� �� ����������� �����
CClientSocket::CClientSocket( int iType, bool bBlocking )throw( SocketErr ):CSocket( iType, bBlocking )
{
}

CClientSocket::~CClientSocket(void)throw( SocketErr )
{
}


//���������� � strAddr:iPort,��� sAddr - ��� ���������� �����,���� ��� IP-������
//								 iPort - ����, � �������� ���������� ������������
void CClientSocket::Connect( std::string strAddr, int iPort )throw( SocketDNSErr, CSocket::SocketErr )
{
	sockaddr_in sAddr;
	hostent* hn;

	::ZeroMemory( &sAddr, sizeof( sAddr ) );
	sAddr.sin_family = AF_INET;

	if( INADDR_NONE != ::inet_addr( strAddr.c_str() ) )
		sAddr.sin_addr.S_un.S_addr = ::inet_addr( strAddr.c_str() );
	else
	{
		//���� ������ �� ����� � ��� ���������� - �������� ����� � ������� DNS
		if( NULL == ( hn = ::gethostbyname( strAddr.c_str() ) ) )
		{
			int iLastError = ::WSAGetLastError();
			if( WSAHOST_NOT_FOUND == iLastError )
				throw SocketDNSErr();
			else
				throw SocketErr( iLastError );
		}
		sAddr.sin_addr.S_un.S_addr = ::inet_addr( hn->h_addr_list[0] );
	}
	sAddr.sin_port = ::htons( iPort );
	//��������� ����������
	if( SOCKET_ERROR == ::connect( m_Socket, (sockaddr*)&sAddr, sizeof( sAddr ) ) )
		throw SocketErr( WSAGetLastError() );
}
