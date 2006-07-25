//-------------------------------------------------------------------------------------
//Этот файл является частью проекта Exspecto 2006г.
//Module: CSocket class
//Author: Parshin Dmitry
//Description: Класс, реализующий взаимодействие с сокетами (общая для клиента и сервера часть)
//-------------------------------------------------------------------------------------
#pragma once
#include <string>
#include "winsock2.h"

class CSocket 
{
public:
	//Конструктор, iType - тип сокета,может быть SOCK_STREAM/SOCK_DGRAM
	//			   bBlocking - тип вызовов, по умолчанию - блокирующие
	CSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	//Конструктор, s - созданный функцией ::socket сокет
	//			   bBlocking - тип вызовов, по умолчанию - блокирующие
	CSocket( SOCKET s, bool bBlocking );

	virtual ~CSocket(void);

	//Метод, возвращающий код последней ошибки
	int GetLastError(void);

	//Метод закрытия сокета
    int Close( void );

	//Метод посылки данных,возвращает SOCKET_ERROR либо кол-во отправленных байт
	int Send( void* pBuffer, int iSize );

	//Метод приёма,возвращает SOCKET_ERROR либо кол-во отправленных байт
	int Receive( void* pBuffer, int iBufSize );

	//Метод, устанавливающий тип вызовов(true - блокирующие,false - неблокирующие )
	void SetBlocking( bool bIsBlocking );

	//При использовании неблокирующих вызовов, метод возвращает true,если в приемный буфер
	//поступили данные и можно производить операцию Receive
	//Timeout - время ожидания (мкс),если -1,бесконечное ожидание
	bool IsReadyForRead( int iTimeout = -1 );

	//При использовании неблокирующих вызовов, метод возвращает true,если сокет готов к
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

	//Тип сокета
	int m_iType;
};
