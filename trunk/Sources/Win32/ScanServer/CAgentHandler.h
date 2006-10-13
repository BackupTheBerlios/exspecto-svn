//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
//-------------------------------------------------------------------------------------//
#ifndef CAGENTHANDLER_H_
#define CAGENTHANDLER_H_

#include <vector>
#include <string>
#include "commands.h"
#include "..\libNet\packet.h"
#include "..\libNet\ClientSocket.h"

#define PORT 5000

class CAgentHandler
{
public:

	//Тип определяющий реакцию агента на команды
	enum enumCommandResult{
		RES_OK = 0x00,			//всё хорошо
		RES_NOT_OPENED,			//видимо неправильно задали адрес,либо агент не запущен
		RES_NO_RESPONSE,		//ответа от агента на команду не последовало
		RES_AGENT_ERR,			//агент не смог выполнить команду
		RES_INCORRECT,			//неверный ответ
		RES_SOCK_ERR			//любая другая ошибка
	};

	virtual ~CAgentHandler();
	
	//Создать обьект взаимодействия с агентом по адресу strAgentAddress 
	CAgentHandler( std::string strAgentAddress );
	
	//Открыть соединение с агентом
	bool Open();
	
	//Закрыть соединение с агентом
	bool Close();
	
	//Команды, передаваемые агенту:
	
	//Начать сканирование
	enumCommandResult BeginScan( std::vector< std::string > vecAddresses );
	
	//Остановить сканирование
	enumCommandResult StopScan();
	
	//Получить статус
	enumCommandResult GetStatus( enumAgentStatus& Status );
	
	//Получить данные последнего сканирования
	enumCommandResult GetData();
	
	//Открыто ли соединение с агентом
	bool IsOpened();
	

protected:
	
	//Отправить пакет Msg агенту и получить ответ в pbRespBuf, iRespSize - ожидаемый размер ответа
	enumCommandResult SendMessage( CPacket &Msg, BYTE* pbRespBuf, int iRespSize );

private:
	
	//Адрес агента
	std::string m_strAddress;
	
	//Сокет для соединения с агентом
	CClientSocket m_Sock;
	
	//Флаг открытия соединения
	bool m_bOpened;
	
};

#endif /*CAGENTHANDLER_H_*/
