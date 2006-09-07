//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции планировщика
//-------------------------------------------------------------------------------------//
#pragma once
#include <string>
#include <map>
#include <vector>
#include "windows.h"

class CScheduler
{
public:
	CScheduler(void);
	~CScheduler(void);

//	void FindAgents( std::vector< std::string > &m_AgentList );
//	void AddAgent( std::string strAddress );
//	void RemoveAgent( int iAgentId );
//	int GetAgentId( std::string strAdress );
//	std::string GetAgentAddress( int iAgentId );

	//Отправить команду Command на адрес strAddress и получить ответ в pBuffer
	//iBufSize - размер буфера
	bool SendCommand( std::string strAddress, enumCommands Command, BYTE* pBuffer = NULL, int iBufSize = 0 );

	//Отправить команду Command на адрес strAddress с параметрами vcParams
	bool SendCommand( std::string strAddress, enumCommands Command, std::vector< std::string > vcParams );

protected:

	//int m_iCurId;

};
