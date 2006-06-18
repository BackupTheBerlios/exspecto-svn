#include "StdAfx.h"
#include ".\socket.h"
#include "winsock2.h"
#include <string>

CSocket::CSocket( int iType, bool bBlocking ):m_iLastError(0)
							 ,m_Socket( INVALID_SOCKET )
							 ,m_bBlocking( bBlocking )
{
	//Инициализация сокетов
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), NULL ) )
	{
		m_iLastError = ::WSAGetLastError();
	}else
	{
		m_iLastError = 0;
	}
	//Создаем сокет
	if( INVALID_SOCKET == ( m_Socket = ::socket( AF_INET, iType, 0 ) ) )
	{
		m_iLastError = ::WSAGetLastError();
	}else
	{
		m_iLastError = 0;
	}
	//Устанавливаем тип вызовов
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

int CSocket::Connect( std::string strAddr, int iPort )
{
	sockaddr_in sAddr;
	hostent* hn;
	int Result;

	::ZeroMemory( &sAddr, sizeof( sAddr ) );
	sAddr.sin_family = AF_INET;

	if( IsAddr( strAddr ) )
		sAddr.sin_addr.S_un.S_addr = ::inet_addr( strAddr.c_str() );
	else
	{
		if( NULL == ( hn = ::gethostbyname( strAddr.c_str() ) ) )
		{
			m_iLastError = ::WSAGetLastError();
			return 0;
		}
		sAddr.sin_addr.S_un.S_addr = ::inet_addr( hn->h_addr_list[0] );
	}
	sAddr.sin_port = ::htons( iPort );
	//Выполняем соединение
	if( SOCKET_ERROR == ( Result = ::connect( m_Socket, (sockaddr*)&sAddr, sizeof( sAddr ) ) ) )
	{
		m_iLastError = ::WSAGetLastError();
		return Result;
	}
	return 0;
}

bool CSocket::IsAddr(std::string strName)
{
	//Алгоритм проверки: вырезаем подстроки между точками и пытаемся сконвертировать их в число
	//если получается и точек не меньше 4, то всё ок,иначе strName - не ip адрес
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