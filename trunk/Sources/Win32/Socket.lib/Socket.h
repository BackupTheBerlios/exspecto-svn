//-------------------------------------------------------------------------------------
//Module: CSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий взаимодействие с сокетами
//-------------------------------------------------------------------------------------
#pragma once
#include "winsock2.h"
#include <string>

class CSocket
{
public:
	//Конструктор, iType - тип сокета,может быть SOCK_STREAM/SOCK_DGRAM
	//			   bBlocking - тип вызовов, по умолчанию - блокирующие
	CSocket( int iType, bool bBlocking = true );
	virtual ~CSocket(void);

	//Функция, возвращающая код последней ошибки
	int GetLastError(void);

	//Соединение с strAddr:iPort,где sAddr - имя удаленного хоста,либо его IP-адресс
	//								 iPort - порт, к которому необходимо подключиться
	int Connect( std::string strAddr, int iPort );

	//Функция проверки: является ли strName ip - адресом
	bool IsAddr(std::string strName);

	//Функция закрытия сокета
    int Close( void );

	//Функция посылки данных,возвращает SOCKET_ERROR либо кол-во отправленных байт
	int Send( void* pBuffer, int iSize );

	//Функция приёма,возвращает SOCKET_ERROR либо кол-во отправленных байт
	int Receive( void* pBuffer, int iBufSize );

	//Функция, устанавливающая тип вызовов(true - блокирующие,false - неблокирующие )
	void SetBlocking( bool bIsBlocking );

	//При использовании неблокирующих вызовов, функци возвращает true,если в приемный буфер
	//поступили данные и можно производить операцию Receive
	//Timeout - время ожидания (мкс),если -1,бесконечное ожидание
	bool IsReadyForRead( int iTimeout = -1 );

	//При использовании неблокирующих вызовов, функци возвращает true,если сокет готов к
	//записи
	//Timeout - время ожидания (мкс),если -1,бесконечное ожидание
	bool IsReadyForWrite( int iTimeout = -1 );

protected:
	//Флаг,указывающий на тип используемых вызовов (блокирующие/неблокирующие)
	bool m_bBlocking;

	//Переменная, содержащая код последней ошибки
	int m_iLastError;

	//Дескриптор сокета, используемый практически во всех функциях
	SOCKET m_Socket;
};
