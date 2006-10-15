//-------------------------------------------------------------------------------------//
//Ётот файл €вл€етс€ частью проекта Exspecto 2006г.
//Module: CServerSocket class
//Author: Parshin Dmitry
//Description:  ласс, реализующий серверную часть сокетов
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include ".\serversocket.h"
#include <string>

// онструктор
//iType - тип сокета, возможные значени€ - SOCK_STREAM и SOCK_DGRAM
//bBlocking - блокирующий либо не блокирующий сокет
CServerSocket::CServerSocket( int iType, bool bBlocking ):CSocket( iType, bBlocking )
{
}

// опирующий конструктор
CServerSocket::CServerSocket( CServerSocket& S ):CSocket( S.m_iType, S.m_bBlocking )
{
	m_Socket = S.m_Socket;
	m_iType = S.m_iType;
}

CServerSocket::~CServerSocket(void)
{
}

//‘ункци€ "прикреплени€" сокета к адресу, по умолчанию ќ— сама выбирает адрес и порт
void CServerSocket::Bind( int iPort, std::string strAddr )throw( SocketDNSErr, SocketErr )
{
	sockaddr_in sAddr;
	hostent* hn;

	::ZeroMemory( &sAddr, sizeof( sAddr ) );
	sAddr.sin_family = AF_INET;
	//Adress	
	if( "Any" == strAddr )
        sAddr.sin_addr.S_un.S_addr =  htonl(INADDR_ANY);
	else if( INADDR_NONE != ::inet_addr( strAddr.c_str() ) )
	{
		sAddr.sin_addr.S_un.S_addr = ::inet_addr( strAddr.c_str() );
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
		sAddr.sin_addr.S_un.S_addr = ::inet_addr( hn->h_addr_list[0] );
	}
	//Port
	sAddr.sin_port = htons( iPort );
    if ( SOCKET_ERROR == bind( m_Socket, (sockaddr* )&sAddr, sizeof ( sAddr ) ) )
		throw SocketErr( WSAGetLastError() );
}

//‘ункци€ начала "прослушивани€", iMaxConn - максимальное кол-во соединени€,
//SOMAXCONN - максимальное значение
void CServerSocket::Listen( int iMaxConn )throw( SocketErr )
{
	if( (int)INVALID_SOCKET == ::listen( m_Socket, iMaxConn ) )
		throw SocketErr( WSAGetLastError() );
}

//ѕри вызове accept, сокет блокируетс€ вплоть до по€влени€ сигнала о вход€щем 
//соединении . ‘ункци€ возвращает 
//новый сокет, который будет использоватьс€ дл€ св€зи с присоединившейс€ машиной 
//(система создаЄт его сама, при успешном соединении). Ѕолее подробные данные 
//о присоединившейс€ машине accept возвращает в параметре addr 
//(тип адреса, IP-адрес, порт).
CSocket* CServerSocket::Accept( structAddr& addr )throw( SocketErr )
{
	SOCKET s;
	CSocket* sock;
	sockaddr_in sAddr;
	int len = sizeof(sAddr);
	hostent* hn;

	ZeroMemory (&sAddr, sizeof (sAddr));
	
	if( INVALID_SOCKET == ( s = ::accept( m_Socket, (sockaddr*)&sAddr, &len ) ) )
	{
			throw SocketErr( WSAGetLastError() );	
	}else
	{
		addr.iPort = ::ntohs( sAddr.sin_port );
		addr.strAddr = ::inet_ntoa( sAddr.sin_addr );
		if( NULL != ( hn = ::gethostbyaddr( (const char*)&sAddr.sin_addr.S_un.S_addr, sizeof( sAddr.sin_addr.S_un.S_addr ), m_iType ) ) )
		{
			addr.strName = hn->h_name;
		}
		sock = new CSocket( s, m_bBlocking );
	}
	return sock;
}
