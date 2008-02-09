//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий взаимодействие с сокетами (общая для клиента и сервера часть)
//-------------------------------------------------------------------------------------

#ifndef CSOCKET_H_
#define CSOCKET_H_

#include "Singleton.hpp"
#include <stdexcept>
#include "CLog.h"
#ifdef WIN32
#include "winsock2.h"
#define socklen_t int
#define ioctl ioctlsocket
#else
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define WSAEMSGSIZE EMSGSIZE
#define WSAEINTR EINTR
#define WSAHOST_NOT_FOUND HOST_NOT_FOUND
typedef int SOCKET;
#endif
#include "Os_Spec.h"


//Исключения, генерируемые CSocket и классами, наследующими от него, и функциями из Tools
class SocketErr: public std::exception
{
public:
	SocketErr( int iLastError )
	{
	    #ifdef WIN32
		if( 0 == FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iLastError, 0, data, sizeof(data), NULL ) )
		#else
		if( 0 == strerror_r( errno, data, sizeof( data ) ) )
		#endif
		{
			strcpy( data, "Error during formating error message" );
			#ifdef WIN32
			Log::instance().Trace( 0, "SocketErr: ошибка: %d", GetLastError() );
			#else
			Log::instance().Trace( 0, "SocketErr: ошибка: %d", errno );
			#endif
		}
	};

	SocketErr( const std::string& Msg )
	{
		strcpy( data, Msg.c_str() );
	};

	virtual ~SocketErr()throw(){};

	virtual const char* what() const throw()
	{
		return data;
	};

private:

	char data[1024];
};


class CSocket
{
public:


	class SocketDNSErr: public SocketErr
	{
		public:
        SocketDNSErr():SocketErr( "Адрес не найден" ){};
		virtual ~SocketDNSErr()throw(){};
	};

	class SocketRespSizeErr: public SocketErr
	{
		public:
			SocketRespSizeErr():SocketErr( "Размер ответа слишком велик" ){};
		virtual ~SocketRespSizeErr()throw(){};
	};

	class SocketConnectionLost: public SocketErr
	{
		public:
			SocketConnectionLost():SocketErr( "Разрыв связи" ){};
		virtual ~SocketConnectionLost()throw(){};
	};

	//структура, описывающая адрес компьютера в сети
	struct structAddr
	{
		std::string strAddr;
		std::string strName;
		int iPort;
	};


	//Конструктор, iType - тип сокета,может быть SOCK_STREAM/SOCK_DGRAM
	//			   bBlocking - тип вызовов, по умолчанию - блокирующие
	CSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	//Конструктор, s - созданный функцией ::socket сокет
	//			   bBlocking - тип вызовов, по умолчанию - блокирующие
	CSocket( SOCKET s, bool bBlocking, bool bConnected );

	virtual ~CSocket(void);

	//Метод закрытия сокета
    void Close( void );

	//Метод посылки данных,возвращает кол-во отправленных байт
	void Send( void* pBuffer, int iSize );

	//Метод приёма данных,возвращает кол-во принятых байт
	int Receive( void* pBuffer, int iBufSize );

	//Метод возврщает адрес удаленного хоста
	structAddr GetRemoteHost();

	//Метод, устанавливающий тип вызовов(true - блокирующие,false - неблокирующие )
	void SetBlocking( bool bIsBlocking );

	bool IsConnected()const;

	//При использовании неблокирующих вызовов, метод возвращает true,если в приемный буфер
	//поступили данные и можно производить операцию Receive
	//Timeout - время ожидания (мкс),если -1,бесконечное ожидание
	bool IsReadyForRead( int iTimeout = -1 );

	//При использовании неблокирующих вызовов, метод возвращает true,если сокет готов к
	//записи
	//Timeout - время ожидания (мкс),если -1,бесконечное ожидание
	bool IsReadyForWrite( int iTimeout = -1 );

	//Получить размер данных в буфере
	bool GetPendingDataSize( u_long& ulSize );

protected:

	void SetConnected( bool bConnected );

	//Дескриптор сокета, используемый практически во всех функциях
	SOCKET m_Socket;

	//Флаг,указывающий на тип используемых вызовов (блокирующие/неблокирующие)
	bool m_bBlocking;

	//Тип сокета
	int m_iType;

private:

	CSocket( const CSocket& );
	CSocket& operator=( const CSocket& );

	bool m_bConnected;
};


#endif
