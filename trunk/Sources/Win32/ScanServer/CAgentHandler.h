//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
//-------------------------------------------------------------------------------------//
#ifndef CAGENTHANDLER_H_
#define CAGENTHANDLER_H_

#include "commands.h"
#include "packet.h"
#include "ClientSocket.h"

#define PORT 5000

class CAgentHandler
{
public:

	//классы исключений, генерируемых CAgentHandler
	class HandlerErr: public std::runtime_error
	{
	public:
	
		HandlerErr( const std::string& strErr )throw():std::runtime_error( strErr ){};
		virtual ~HandlerErr()throw(){};	
	}; 
	
	virtual ~CAgentHandler();
	
	//Создать обьект взаимодействия с агентом по адресу strAgentAddress 
	CAgentHandler( std::string strAgentAddress );
	
	//Открыть соединение с агентом
	void Open();
	
	//Закрыть соединение с агентом
	void Close();
	
	//Команды, передаваемые агенту:
	
	//Начать сканирование
	enumAgentResponse BeginScan( std::vector< std::string > vecAddresses )throw( HandlerErr, CSocket::SocketErr );
	
	//Остановить сканирование
	enumAgentResponse StopScan()throw( HandlerErr, CSocket::SocketErr );
	
	//Получить статус
	enumAgentResponse GetStatus( enumAgentState& Status )throw( HandlerErr, CSocket::SocketErr );
	
	//Получить данные последнего сканирования
	enumAgentResponse GetData()throw( HandlerErr, CSocket::SocketErr );
	
	//Открыто ли соединение с агентом
	bool IsOpened()const;
	

protected:
	
	//Отправить пакет Msg агенту и получить ответ в pbRespBuf, iRespSize - ожидаемый размер ответа
	void SendMessage( CPacket &Msg, BYTE* pbRespBuf, int iRespSize )throw( HandlerErr, CSocket::SocketErr );

private:
	
	//Адрес агента
	std::string m_strAddress;
	
	//Сокет для соединения с агентом
	CClientSocket m_Sock;
	
	//Флаг открытия соединения
	bool m_bOpened;
	
};

#endif /*CAGENTHANDLER_H_*/
