#include "stdafx.h"
#include ".\sheduler.h"


CSheduler::CSheduler(void)
{
}

CSheduler::~CSheduler(void)
{
}

void CSheduler::FindAgents()
{
}

void CSheduler::AddAgent( std::string strAddress )
{
	m_iCurId++;
	m_AgentList[ m_iCurId ] = strAddress;
}

void CSheduler::RemoveAgent( int iAgentId )
{
}

int CSheduler::GetAgentId( std::string strAdress )
{
	return int();
}

std::string CSheduler::GetAgentAddress( int iAgentId )
{
	return std::string();
}

bool CSheduler::SendCommand( std::string strAdress, enumCommands Command )
{
	return bool();
}

bool CSheduler::SendCommand( int iAgentId, enumCommands Command )
{
	return bool();
}

int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}