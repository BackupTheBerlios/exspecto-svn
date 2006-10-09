//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
//-------------------------------------------------------------------------------------//
#ifndef CAGENTHANDLER_H_
#define CAGENTHANDLER_H_

#include <string>
#include "commands.h"
#include "..\libNet\packet.h"
#include "..\libNet\ClientSocket.h"

#define PORT 5000

class CAgentHandler
{
public:

	virtual ~CAgentHandler();
	CAgentHandler( std::string strAgentAddress );
	
	bool Open();
	
	bool Close();
	

protected:
	
	enumCommandResult SendMessage( CPacket &Msg );

private:
	
	std::string m_strAddress;
	
	CClientSocket m_Sock;
	
};

#endif /*CAGENTHANDLER_H_*/
