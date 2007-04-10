//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CServerSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий серверную часть сокетов
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "serversocket.h"

//Конструктор
//iType - тип сокета, возможные значения - SOCK_STREAM и SOCK_DGRAM
//bBlocking - блокирующий либо не блокирующий сокет
CServerSocket::CServerSocket( int iType, bool bBlocking ):CSocket( iType, bBlocking )
{
}

//Копирующий конструктор
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

//Функция "прикрепления" сокета к адресу, по умолчанию ОС сама выбирает адрес и порт
void CServerSocket::Bind( int iPort, std::string strAddr )
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

//Функция начала "прослушивания", iMaxConn - максимальное кол-во соединения,
//SOMAXCONN - максимальное значение
void CServerSocket::Listen( int iMaxConn )
{
	if( (int)INVALID_SOCKET == ::listen( m_Socket, iMaxConn ) )
		throw SocketErr( WSAGetLastError() );
}

//При вызове accept, сокет блокируется вплоть до появления сигнала о входящем 
//соединении . Функция возвращает 
//новый сокет, который будет использоваться для связи с присоединившейся машиной 
//(система создаёт его сама, при успешном соединении). Более подробные данные 
//о присоединившейся машине accept возвращает в параметре addr 
//(тип адреса, IP-адрес, порт).
SmartPtr< CSocket > CServerSocket::Accept( structAddr& addr )
{
	SOCKET s;
	SmartPtr< CSocket > sock;
	sockaddr_in sAddr;
	int len = sizeof(sAddr);
	hostent* hn;
	ZeroMemory (&sAddr, sizeof (sAddr));
	SetConnected( true );
	if( INVALID_SOCKET == ( s = ::accept( m_Socket, (sockaddr*)&sAddr, &len ) ) )
	{
		SetConnected( false );
		int iLastErr;
		//Если ожидание отменено - выходим без ошибки
		if( WSAEINTR == ( iLastErr = WSAGetLastError() ) )
			return sock;
		throw SocketErr( WSAGetLastError() );	
	}else
	{
		addr.iPort = ::ntohs( sAddr.sin_port );
		addr.strAddr = ::inet_ntoa( sAddr.sin_addr );
		if( NULL != ( hn = ::gethostbyaddr( (const char*)&sAddr.sin_addr.S_un.S_addr, sizeof( sAddr.sin_addr.S_un.S_addr ), m_iType ) ) )
		{
			addr.strName = hn->h_name;
		}
		sock = SmartPtr< CSocket >( new CSocket( s, m_bBlocking, true ) );
	}
	SetConnected( false );
	return sock;
}
