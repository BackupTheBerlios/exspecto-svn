#pragma once
#include "winsock2.h"
#include <string>

class CSocket
{
public:
	CSocket( int iType );
	virtual ~CSocket(void);

	//Функция, возвращающая код последней ошибки
	int GetLastError(void);

	//Соединение с strAddr:iPort,где sAddr - имя удаленного хоста,либо его IP-адресс
	//								 iPort - порт, к которому необходимо подключиться
	int Connect( std::string strAddr, int iPort );

	//Функция проверки: является ли strName ip - адресом
	bool IsAddr(std::string strName);

protected:
	
	int m_iLastError;
	//Дескриптор сокета, используемый практически во всех функциях
	SOCKET m_Socket;
};
