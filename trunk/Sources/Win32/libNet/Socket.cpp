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
CSocket::CSocket( SOCKET s, bool bBlocking, bool bConnected ):m_bBlocking( bBlocking )
{
	WSADATA WSAData;
	//Инициализация сокетов
	if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )
	{
		throw SocketErr( WSAGetLastError() );
	}

	SetConnected( bConnected );
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
	WSACleanup();
}

//Метод закрытия сокета
void CSocket::Close( void )
{
	//TODO:разобраться что происходит при вызове shutdown если соединение не установлено либо разорвано
	if( INVALID_SOCKET != m_Socket )
	{
		if( SOCKET_ERROR == ::closesocket( m_Socket ) )
			throw SocketErr( WSAGetLastError() );
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
		::ioctlsocket( m_Socket, FIONBIO, (unsigned long* )&l );
	}else
	{
		::ioctlsocket( m_Socket, FIONBIO, (unsigned long* )&l );
	}
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
	
	if( SOCKET_ERROR == getpeername( m_Socket, (sockaddr*)&sAddr, &len ) )
		throw SocketErr( WSAGetLastError() );
	else
	{
		SetConnected( true );
		res.iPort = ::ntohs( sAddr.sin_port );
		res.strAddr = ::inet_ntoa( sAddr.sin_addr );
		if( NULL != ( hn = ::gethostbyaddr( (const char*)&sAddr.sin_addr.S_un.S_addr, sizeof( sAddr.sin_addr.S_un.S_addr ), m_iType ) ) )
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
	if( ioctlsocket( m_Socket, FIONREAD, &ulSize ) == -1 ) return false;
	return true; 
}

namespace Tools{

	CPingHelper::CPingHelper()
	{
		WSADATA WSAData;
		//Инициализация сокетов
		if( 0 != ::WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )
			throw SocketErr( WSAGetLastError() );

		m_hLib = LoadLibrary("ICMP.DLL");
		if( NULL == m_hLib )
			throw SocketErr( GetLastError() );

		m_pIcmpCreateFile = (pfnHV)GetProcAddress( m_hLib, "IcmpCreateFile" );
		if( NULL == m_pIcmpCreateFile )
		{
			FreeLibrary( m_hLib );
            throw SocketErr( GetLastError() );
		}
		m_pIcmpCloseHandle = (pfnBH)GetProcAddress( m_hLib, "IcmpCloseHandle" );
		if( NULL == m_pIcmpCloseHandle )
		{
			FreeLibrary( m_hLib );
            throw SocketErr( GetLastError() );
		}

		m_pIcmpSendEcho = (pfnDHDPWPipPDD)GetProcAddress( m_hLib, "IcmpSendEcho" );
		if( NULL == m_pIcmpSendEcho )
		{
			FreeLibrary( m_hLib );
            throw SocketErr( GetLastError() );
		}

	}

	CPingHelper::~CPingHelper()
	{
		FreeLibrary( m_hLib );
		WSACleanup();
	}

	bool CPingHelper::Ping( const std::string& strHost, unsigned int iTimeout, unsigned int iRequestCount )
	{
		char acPingBuffer[32]={0};  // буфер для передачи
		struct in_addr DestAddress;
		struct hostent* pHostEnt;
		// Выделяем память под пакет
		PIP_ECHO_REPLY pIpe = (PIP_ECHO_REPLY)new BYTE[ sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer)];
		if (pIpe == 0) {
			throw SocketErr( GetLastError() );
		}


		// получаем адрес хоста, который надо пингануть
		DestAddress.s_addr = inet_addr(strHost.c_str());
		if (DestAddress.s_addr == INADDR_NONE)
			pHostEnt = gethostbyname(strHost.c_str());
		else
			pHostEnt = gethostbyaddr((const char *)&DestAddress, sizeof(struct in_addr), AF_INET);

		if (pHostEnt == NULL) {
			return false;
		}

		HANDLE hIP;
		// Пытаемся создать файл сервиса
		hIP = m_pIcmpCreateFile();

		if (hIP == INVALID_HANDLE_VALUE) {
			delete[] pIpe;
			throw SocketErr( GetLastError() );
		}



		pIpe->Data = acPingBuffer;
		pIpe->DataSize = sizeof(acPingBuffer);

		bool bRes = true;
		DWORD dwStatus;
		for(unsigned int c=0;c<iRequestCount;c++) {
			dwStatus = m_pIcmpSendEcho( hIP, *((unsigned int*)pHostEnt->h_addr_list[0]),
				acPingBuffer, sizeof(acPingBuffer), NULL, pIpe,
				sizeof(IP_ECHO_REPLY) + sizeof(acPingBuffer), iTimeout );
			bRes &= (dwStatus == 0)?false:true;
		}
		m_pIcmpCloseHandle(hIP);
		delete[] pIpe;
		return bRes;
	}

}
