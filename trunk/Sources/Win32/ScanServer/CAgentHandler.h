//------------------------------------------------------------------------------------//
//Ётот файл €вл€етс€ частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description:  ласс, инкапсулирующий взаимодействие с агентом
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

	//“ип определ€ющий реакцию агента на команды
	enum enumCommandResult{
		RES_OK = 0x00,			//всЄ хорошо
		RES_NOT_OPENED,			//видимо неправильно задали адрес,либо агент не запущен
		RES_NO_RESPONSE,		//ответа от агента на команду не последовало
		RES_AGENT_ERR,			//агент не смог выполнить команду
		RES_INCORRECT,			//неверный ответ
		RES_SOCK_ERR			//люба€ друга€ ошибка
	};

	virtual ~CAgentHandler();
	
	CAgentHandler( std::string strAgentAddress );
	
	bool Open();
	
	bool Close();
	
	enumCommandResult BeginScan( std::vector< std::string > vecAddresses );
	
	enumCommandResult StopScan();
	
	enumCommandResult GetStatus( enumAgentStatus& Status );
	
	enumCommandResult GetData();
	
	bool IsOpened();
	

protected:
	
	enumCommandResult SendMessage( CPacket &Msg, BYTE* pbRespBuf, int iRespSize );

private:
	
	std::string m_strAddress;
	
	CClientSocket m_Sock;
	
	bool m_bOpened;
	
};

#endif /*CAGENTHANDLER_H_*/
