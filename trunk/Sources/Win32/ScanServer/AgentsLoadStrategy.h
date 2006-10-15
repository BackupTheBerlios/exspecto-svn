//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Agents load strategy
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� �������� ������� � ����� Container
//-------------------------------------------------------------------------------------//

#ifndef AGENTSLOADSTRATEGY_H_
#define AGENTSLOADSTRATEGY_H_

#include "precomp.h"
#include "CAgentHandler.h"

class AgentsLoadStrategy
{
public:
	AgentsLoadStrategy( std::vector< CAgentHandler* >& vecStorage );
	virtual ~AgentsLoadStrategy();
private:

	//������ ���������� �� ��������� � �������� �������� ������ ��� ����������� �� ��������
	std::vector< CAgentHandler* > m_ReleseCont;
};

#endif /*AGENTSLOADSTRATEGY_H_*/
