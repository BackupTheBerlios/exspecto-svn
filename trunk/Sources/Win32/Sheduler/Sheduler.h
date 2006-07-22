#pragma once
#include "../common/commands.h"
#include <string>
#include <map>
#include <vector>
#include "windows.h"

class CSheduler
{
public:
	CSheduler(void);
	~CSheduler(void);

	void FindAgents( std::vector< std::string > &m_AgentList );
//	void AddAgent( std::string strAddress );
//	void RemoveAgent( int iAgentId );
//	int GetAgentId( std::string strAdress );
//	std::string GetAgentAddress( int iAgentId );

	bool SendCommand( std::string strAddress, enumCommands Command, BYTE* pBuffer = NULL, int iBufSize = 0 );

	bool SendCommand( std::string strAddress, enumCommands Command, std::vector< std::string > vcParams );

protected:

	int m_iCurId;

	
};
