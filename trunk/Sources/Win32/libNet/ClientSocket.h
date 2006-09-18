//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий клиентскую часть сокетов
//-------------------------------------------------------------------------------------//
#pragma once
#include "Socket.h"
#include <string>

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
	int Connect( std::string strAddr, int iPort );
};
