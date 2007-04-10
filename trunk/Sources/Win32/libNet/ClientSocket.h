//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий клиентскую часть сокетов
//-------------------------------------------------------------------------------------//
#ifndef CCLIENTSOCKET_H_
#define CCLIENTSOCKET_H_

#include "Socket.h"

class CClientSocket: public CSocket
{
public:

	//Конструктор
	//iType - тип сокета, возможные значения - SOCK_STREAM и SOCK_DGRAM
	//bBlocking - блокирующий либо не блокирующий сокет
	CClientSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	~CClientSocket( void );


	//Соединение с strAddr:iPort,где sAddr - имя удаленного хоста,либо его IP-адресс
	//								 iPort - порт, к которому необходимо подключиться
	void Connect( std::string strAddr, int iPort );

private:
	CClientSocket( const CClientSocket& );
	CClientSocket& operator=( const CClientSocket& );
};

#endif
