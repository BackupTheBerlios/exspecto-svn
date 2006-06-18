//-------------------------------------------------------------------------------------
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий серверную часть сокетов
//-------------------------------------------------------------------------------------
#pragma once
#include "Socket.h"
#include <string>

class CServerSocket: public CSocket
{
public:
	CServerSocket( int iType, bool bBlocking );
	~CServerSocket(void);

	//Функция "прикрепления" сокета к адресу, по умолчанию ОС сама выбирает адрес и порт
	int Bind( int iPort = 0, std::string strAddr = "Any" );

protected:
};
