﻿//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CServerSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий серверную часть сокетов
//-------------------------------------------------------------------------------------//
#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include "Socket.h"
#include "SmartPtr.hpp"

class CServerSocket: public CSocket
{
public:

	//Конструктор
	//iType - тип сокета, возможные значения - SOCK_STREAM и SOCK_DGRAM
	//bBlocking - блокирующий либо не блокирующий сокет
	CServerSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	CServerSocket( const CServerSocket& S );

	CServerSocket& operator=( const CServerSocket& );


	~CServerSocket(void);

	//Функция "прикрепления" сокета к адресу, по умолчанию ОС сама выбирает адрес и порт
	void Bind( int iPort = 0, std::string strAddr = "Any" );

	//Функция начала "прослушивания", iMaxConn - максимальное кол-во соединения,
	//SOMAXCONN - максимальное значение
	void Listen( int iMaxConn = SOMAXCONN );

	//При вызове accept, сокет блокируется вплоть до появления сигнала о входящем 
	//соединении (по аналогии к функциям из предыдущей статьи). Функция возвращает 
	//новый сокет, который будет использоваться для связи с присоединившейся машиной 
	//(система создаёт его сама, при успешном соединении). Более подробные данные 
	//о присоединившейся машине accept возвращает в параметре addr 
	//(тип адреса, IP-адрес, порт).
	SmartPtr< CSocket > Accept( structAddr& addr );
private:

};

#endif
