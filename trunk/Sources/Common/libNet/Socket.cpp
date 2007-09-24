//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий взаимодействие с сокетами (общая для клиента и сервера часть)
//-------------------------------------------------------------------------------------
#include "Socket.h"
#include "SmartPtr.hpp"

//Конструктор, iType - тип сокета,может быть SOCK_STREAM/SOCK_DGRAM
//			   bBlocking - тип вызовов, по умолчанию - блокирующие
CSocket::CSocket( int iType, bool bBlocking ):m_Socket( INVALID_SOCKET ),
											 					m_bBlocking( bBlocking ),
											 					m_iType( iType ),
											 					m_bConnected( false )
{
    #ifdef WIN32
	WSADATA WSAData;
	 //Инициализация сокетов
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )
		throw SocketErr( WSAGetLastError() );
    #endif
	//Создаем сокет
	if( INVALID_SOCKET == ( m_Socket = ::socket( AF_INET, iType, 0 ) ) )
		throw SocketErr( WSAGetLastError() );

	//Устанавливаем тип вызовов
	SetBlocking( bBlocking );
}

//Конструктор, s - созданный функцией ::socket сокет
//			   bBlocking - тип вызовов, по умолчанию - блокирующие
CSocket::CSocket( SOCKET s, bool bBlocking, bool bConnected ):m_bBlocking( bBlocking )
{
    #ifdef WIN32
	WSADATA WSAData;
	//Инициализация сокетов
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
		Log::instance().Trace( 50, "CSocket::~CSocket: Возникло исключение: %s", e.what() );
	};
	#ifdef WIN32
	WSACleanup();
	#endif
}

//Метод закрытия сокета
void CSocket::Close( void )
{
	//TODO:разобраться что происходит при вызове shutdown если соединение не установлено либо разорвано
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

//Метод, устанавливающий тип вызовов(true - блокирующие,false - неблокирующие )
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

//Метод посылки данных,возвращает SOCKET_ERROR либо кол-во отправленных байт
void CSocket::Send( void* pBuffer, int iSize )
{
	int res;
	if( SOCKET_ERROR == ( res = ::send( m_Socket, (const char*)pBuffer, iSize, 0 ) ) )
		throw SocketErr( WSAGetLastError() );
	if( res != iSize )
	{
		Log::instance().Trace( 1, "CSocket::Send: Количество посланных и фактически отправленных байт не совпадает" );
		throw SocketErr( "Количество посланных и фактически отправленных байт не совпадает" );
	}
	SetConnected( true );
}

//Метод приёма,возвращает SOCKET_ERROR либо кол-во полученных байт
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

//Метод возврщает адрес удаленного хоста
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
		res.iPort = ::ntohs( sAddr.sin_port );
		res.strAddr = ::inet_ntoa( sAddr.sin_addr );
		if( NULL != ( hn = ::gethostbyaddr( (const char*)&sAddr.sin_addr.s_addr, sizeof( sAddr.sin_addr.s_addr ), m_iType ) ) )
			res.strName = hn->h_name;
		return res;
	}
}

//При использовании неблокирующих вызовов, метод возвращает true,если в приемный буфер
//поступили данные и можно производить операцию Receive
//Timeout - время ожидания (мкс),если -1,бесконечное ожидание
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

//При использовании неблокирующих вызовов, метод возвращает true,если сокет готов к
//записи
//Timeout - время ожидания (мкс),если -1,бесконечное ожидание
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

