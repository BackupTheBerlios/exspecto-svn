//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий клиентскую часть сокетов
//-------------------------------------------------------------------------------------//
#ifndef CCLIENTSOCKET_H_
#define CCLIENTSOCKET_H_

#include "precomp.h"
#include "Socket.h"

class CClientSocket: public CSocket
{
public:

	//Конструктор
	//iType - тип сокета, возможные значения - SOCK_STREAM и SOCK_DGRAM
	//bBlocking - блокирующий либо не блокирующий сокет
	CClientSocket( int iType = SOCK_STREAM, bool bBlocking = true )throw( SocketErr );

	~CClientSocket( void )throw( SocketErr );


	//Соединение с strAddr:iPort,где sAddr - имя удаленного хоста,либо его IP-адресс
	//								 iPort - порт, к которому необходимо подключиться
	void Connect( std::string strAddr, int iPort )throw( SocketDNSErr, CSocket::SocketErr );
};

#endif
