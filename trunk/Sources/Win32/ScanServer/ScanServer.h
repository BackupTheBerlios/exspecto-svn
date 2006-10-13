//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������
//-------------------------------------------------------------------------------------//
#include "windows.h"
#include <string>
#include <map>
#include <vector>
#include "CAgentHandler.h"
#include "CStartTrigger.h"
#include "../libCommon/Container.hpp"
#include "AgentsLoadStrategy.h"
#include "CStartScanEventInterface.h"


class CScanServer: public CStartScanEventInterface 
{
public:
	CScanServer(void);
	virtual ~CScanServer(void);
	
	virtual void OnStartScan();

private:

	Container< CAgentHandler*, AgentsLoadStrategy > m_AgentsContainer;
	
	CStartTrigger* m_pTrigger;

};
