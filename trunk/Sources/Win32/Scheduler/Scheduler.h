//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������
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

	//��������� ������� Command �� ����� strAddress � �������� ����� � pBuffer
	//iBufSize - ������ ������
	bool SendCommand( std::string strAddress, enumCommands Command, BYTE* pBuffer = NULL, int iBufSize = 0 );

	//��������� ������� Command �� ����� strAddress � ����������� vcParams
	bool SendCommand( std::string strAddress, enumCommands Command, std::vector< std::string > vcParams );

protected:

	//int m_iCurId;

};
