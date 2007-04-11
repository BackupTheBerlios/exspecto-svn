//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������
//-------------------------------------------------------------------------------------//

#ifndef CSCHEDULER_H_
#define CSCHEDULER_H_
#include "ServerSocket.h"
#include "CAgentHandler.h"
#include "CStartTrigger.h"
#include "Container.hpp"
#include "AgentsLoadStrategy.h"
#include "SmartPtr.hpp"
#include "Event.hpp"
#include "ThreadsPool.h"



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

	//����� ���������� ��������� ��� ������������
	virtual void OnStartScan();

private:
	CScheduler( const CScheduler& );
	CScheduler& operator=( const CScheduler& );

	//���������, ���������� ������� ���������� ��������, ����������� ���������� AgentsLoadStrategy
	std::map< std::string, SmartPtr< CAgentHandler > > m_mapAgentsContainer;
	
	//�������, ������������� ��� ������������� ������ ������������
	std::auto_ptr< CStartTrigger > m_pTrigger;

	static unsigned _stdcall fnListenThreadProc(  void* pParameter );

	CEvent m_CloseEv;
	
	HANDLE m_hListenThread;
	
	CServerSocket m_EventSock;

};

#endif //CSCHEDULER_H_
