//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий взаимодействие с сокетами (общая для клиента и сервера часть)
//-------------------------------------------------------------------------------------

#ifndef CSOCKET_H_
#define CSOCKET_H_

#include "precomp.h"
#include <string>
#include <stdexcept>

class CSocket 
{
public:

	//Исключения, генерируемые CSocket и классами, наследующими от него
	class SocketErr: public std::exception
	{
	public:
		SocketErr( int iLastError )throw()
		{
			if( 0 == FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, iLastError, 0, data, sizeof(data), NULL ) )
			{
				strcpy( data, "Error during formating error message" );
				Log::instance().Trace( 0, "SocketErr: ошибка: %d", GetLastError() );
			} 
		};
		
		virtual ~SocketErr()throw(){};
		
		virtual const char* what() const throw()
		{
		 	return data;	 
		};
		
	private:
	
		char data[1024];
	};

	class SocketDNSErr: public CSocket::SocketErr 
	{
		public:
			SocketDNSErr():CSocket::SocketErr( WSAHOST_NOT_FOUND ){};
		virtual ~SocketDNSErr()throw(){};
	};
	
	class SocketRespSizeErr: public CSocket::SocketErr 
	{
		public:
			SocketRespSizeErr():CSocket::SocketErr( WSAEMSGSIZE ){};
		virtual ~SocketRespSizeErr()throw(){};
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
	CSocket( int iType = SOCK_STREAM, bool bBlocking = true )throw( SocketErr );

	//Конструктор, s - созданный функцией ::socket сокет
	//			   bBlocking - тип вызовов, по умолчанию - блокирующие
	CSocket( SOCKET s, bool bBlocking )throw( SocketErr );

	virtual ~CSocket(void);

	//Метод закрытия сокета
    void Close( void )throw( SocketErr );

	//Метод посылки данных,возвращает кол-во отправленных байт
	int Send( void* pBuffer, int iSize )throw( SocketErr );

	//Метод приёма данных,возвращает кол-во принятых байт
	int Receive( void* pBuffer, int iBufSize )throw( SocketErr );
	
	//Метод возврщает адрес удаленного хоста
	structAddr GetRemoteHost(); 

	//Метод, устанавливающий тип вызовов(true - блокирующие,false - неблокирующие )
	void SetBlocking( bool bIsBlocking );

	//При использовании неблокирующих вызовов, метод возвращает true,если в приемный буфер
	//поступили данные и можно производить операцию Receive
	//Timeout - время ожидания (мкс),если -1,бесконечное ожидание
	bool IsReadyForRead( int iTimeout = -1 )throw( SocketErr );

	//При использовании неблокирующих вызовов, метод возвращает true,если сокет готов к
	//записи
	//Timeout - время ожидания (мкс),если -1,бесконечное ожидание
	bool IsReadyForWrite( int iTimeout = -1 )throw( SocketErr );

protected:

	//Дескриптор сокета, используемый практически во всех функциях
	SOCKET m_Socket;

	//Флаг,указывающий на тип используемых вызовов (блокирующие/неблокирующие)
	bool m_bBlocking;

	//Тип сокета
	int m_iType;
};

#endif
