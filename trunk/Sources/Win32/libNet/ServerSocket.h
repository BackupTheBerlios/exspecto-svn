//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CServerSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий серверную часть сокетов
//-------------------------------------------------------------------------------------//
#pragma once
#include "Socket.h"
#include <string>

//структура, описывающая адрес компьютера в сети
struct structAddr
{
	std::string strAddr;
	std::string strName;
	int iPort;
};

class CServerSocket: public CSocket
{
public:
	//Конструктор
	//iType - тип сокета, возможные значения - SOCK_STREAM и SOCK_DGRAM
	//bBlocking - блокирующий либо не блокирующий сокет
	CServerSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	//Копирующий конструктор
	CServerSocket( CServerSocket& S );

	~CServerSocket(void);

	//Функция "прикрепления" сокета к адресу, по умолчанию ОС сама выбирает адрес и порт
	int Bind( int iPort = 0, std::string strAddr = "Any" );

	//Функция начала "прослушивания", iMaxConn - максимальное кол-во соединения,
	//SOMAXCONN - максимальное значение
	int Listen( int iMaxConn = SOMAXCONN );

	//При вызове accept, сокет блокируется вплоть до появления сигнала о входящем 
	//соединении (по аналогии к функциям из предыдущей статьи). Функция возвращает 
	//новый сокет, который будет использоваться для связи с присоединившейся машиной 
	//(система создаёт его сама, при успешном соединении). Более подробные данные 
	//о присоединившейся машине accept возвращает в параметре addr 
	//(тип адреса, IP-адрес, порт).
	CSocket* Accept( structAddr& addr );
protected:
};
