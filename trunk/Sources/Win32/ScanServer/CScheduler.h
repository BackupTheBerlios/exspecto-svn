//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������
//-------------------------------------------------------------------------------------//

#ifndef CSCHEDULER_H_
#define CSCHEDULER_H_

#include "windows.h"
#include <string>
#include <map>
#include <vector>
#include "CAgentHandler.h"
#include "CStartTrigger.h"
#include "../libCommon/Container.hpp"
#include "AgentsLoadStrategy.h"

//��������������� ���������� ������ CStartTrigger
class CStartTrigger;

//�����, ����������� ��������� ��� ������� ��������� ������ OnStartScanEvent,
//���������� � CScheduler ��� ������������ �������� CStartTrigger
class CStartScanEventInterface
{
public:

	virtual void OnStartScan() = 0;
};

class CScheduler: public CStartScanEventInterface 
{
public:
	CScheduler(void);
	virtual ~CScheduler(void);
	
	virtual void OnStartScan();

private:

	//���������, ���������� ������� ���������� ��������, ����������� ���������� AgentsLoadStrategy
	Container< CAgentHandler*, AgentsLoadStrategy > m_AgentsContainer;
	
	//�������, ������������� ��� ������������� ������ ������������
	CStartTrigger* m_pTrigger;

};

#endif //CSCHEDULER_H_
