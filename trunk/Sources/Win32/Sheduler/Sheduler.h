#pragma once
#include "../common/commands.h"
#include <string>
#include <map>

class CSheduler
{
public:
	CSheduler(void);
	~CSheduler(void);

	void FindAgents();
	void AddAgent( std::string strAddress );
	void RemoveAgent( int iAgentId );
	int GetAgentId( std::string strAdress );
	std::string GetAgentAddress( int iAgentId );

	bool SendCommand( std::string strAdress, enumCommands Command );
	bool SendCommand( int iAgentId, enumCommands Command );

protected:

	int m_iCurId;

	std::map< int, std::string > m_AgentList;
};