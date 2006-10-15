//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий клиентскую часть сокетов
//-------------------------------------------------------------------------------------//
#include "clientsocket.h"

//Конструктор
//iType - тип сокета, возможные значения - SOCK_STREAM и SOCK_DGRAM
//bBlocking - блокирующий либо не блокирующий сокет
CClientSocket::CClientSocket( int iType, bool bBlocking )throw( SocketErr ):CSocket( iType, bBlocking )
{
}

CClientSocket::~CClientSocket(void)throw( SocketErr )
{
}


//Соединение с strAddr:iPort,где sAddr - имя удаленного хоста,либо его IP-адресс
//								 iPort - порт, к которому необходимо подключиться
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
		//если указан не адрес а имя компьютера - получаем адрес с помощью DNS
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
	//Выполняем соединение
	if( SOCKET_ERROR == ::connect( m_Socket, (sockaddr*)&sAddr, sizeof( sAddr ) ) )
		throw SocketErr( WSAGetLastError() );
}
