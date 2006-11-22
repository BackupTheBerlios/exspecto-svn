//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий взаимодействие с сокетами (общая для клиента и сервера часть)
//-------------------------------------------------------------------------------------
#include "Socket.h"

//Конструктор, iType - тип сокета,может быть SOCK_STREAM/SOCK_DGRAM
//			   bBlocking - тип вызовов, по умолчанию - блокирующие
CSocket::CSocket( int iType, bool bBlocking )throw( SocketErr ):m_Socket( INVALID_SOCKET ),
											 					m_bBlocking( bBlocking ),
											 					m_iType( iType )
{
	WSADATA WSAData;
	 //Инициализация сокетов
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )
		throw SocketErr( WSAGetLastError() );

	//Создаем сокет
	if( INVALID_SOCKET == ( m_Socket = ::socket( AF_INET, iType, 0 ) ) )
		throw SocketErr( WSAGetLastError() );

	//Устанавливаем тип вызовов
	SetBlocking( bBlocking );
}

//Конструктор, s - созданный функцией ::socket сокет
//			   bBlocking - тип вызовов, по умолчанию - блокирующие
CSocket::CSocket( SOCKET s, bool bBlocking )throw( SocketErr ):m_bBlocking( bBlocking )
{
	WSADATA WSAData;
	//Инициализация сокетов
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
		Log::instance().Trace( 50, "CSocket::~CSocket: Возникло исключение: %s", e.what() );
	};
}

//Метод закрытия сокета
void CSocket::Close( void )throw( SocketErr )
{
	//TODO:разобраться что происходит при вызове shutdown если соединение не установлено либо разорвано
	if( INVALID_SOCKET != m_Socket )
	{
		// SOCKET_ERROR == ::shutdown( m_Socket, SD_BOTH ) ||SOCKET_ERROR == ::closesocket( m_Socket ) 
		if(SOCKET_ERROR == ::shutdown( m_Socket, SD_BOTH )  )
			throw SocketErr( WSAGetLastError() );
		else
			m_Socket = INVALID_SOCKET;
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
		::ioctlsocket( m_Socket, FIONBIO, (unsigned long* )&l );
	}else
	{
		::ioctlsocket( m_Socket, FIONBIO, (unsigned long* )&l );
	}
}

//Метод посылки данных,возвращает SOCKET_ERROR либо кол-во отправленных байт
int CSocket::Send( void* pBuffer, int iSize )throw( SocketErr )
{
	int res;
	if( SOCKET_ERROR == ( res = ::send( m_Socket, (const char*)pBuffer, iSize, 0 ) ) )
		throw SocketErr( WSAGetLastError() );
	return res;
}

//Метод приёма,возвращает SOCKET_ERROR либо кол-во отправленных байт
int CSocket::Receive( void* pBuffer, int iBufSize )throw( SocketErr )
{
	int res;
	if( SOCKET_ERROR == ( res = ::recv( m_Socket, (char*)pBuffer, iBufSize, 0 ) ) )
	{
		int iLastError = WSAGetLastError();
		if( WSAEMSGSIZE == iLastError )
			throw SocketRespSizeErr();
		else
			throw SocketErr( WSAGetLastError() );
	}
	return res;
}

//При использовании неблокирующих вызовов, метод возвращает true,если в приемный буфер
//поступили данные и можно производить операцию Receive
//Timeout - время ожидания (мкс),если -1,бесконечное ожидание
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

//При использовании неблокирующих вызовов, метод возвращает true,если сокет готов к
//записи
//Timeout - время ожидания (мкс),если -1,бесконечное ожидание
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

