//-------------------------------------------------------------------------------------//
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �������ۜ��М������, �������՜��М��ۜ��؜��ל�����������؜��� ���������ݜ��ڜ�����؜��� ���ߜ��ۜ��М��ݜ��؜������ޜ��Ҝ�����؜��ڜ���
//-------------------------------------------------------------------------------------//

#ifndef CSCHEDULER_H_
#define CSCHEDULER_H_
#include "ServerSocket.h"
#include "CAgentHandler.h"
#include "CStartTrigger.h"
#include "AgentsLoadStrategy.h"
#include "SmartPtr.hpp"
#include "Event.h"
#include "ThreadsPool.h"



//�����������՜��Ԝ��Ҝ��М������؜�����՜��ۜ�����ݜ��ޜ��� ���ޜ��ќ��������Ҝ��ۜ��՜��ݜ��؜��� ���ڜ��ۜ��М��������� CStartTrigger
class CStartTrigger;

//�������ۜ��М������, ���ޜ��ߜ��؜��������Ҝ��М��������؜��� ���؜��ݜ�����՜���������՜��ٜ��� ���Ԝ��ۜ��� ���������ݜ��ڜ�����؜��� ���ޜ��ќ������М�����ݜ��ޜ��Ӝ��� ���Ҝ�����ל��ޜ��Ҝ��� OnStartScanEvent,
//���Ҝ�����ל�����Ҝ��М��՜��ܜ��ޜ��� ���� CScheduler ���ߜ������� ����������М��ќ��М��������Ҝ��М��ݜ��؜��� ����������؜��Ӝ��Ӝ��՜������� CStartTrigger
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

	//�������՜�����ޜ��� ���Ҝ�����ל�����Ҝ��М��՜��������� ����������؜��Ӝ��Ӝ��՜������ޜ��� ���ߜ������� ����������М��ќ��М��������Ҝ��М��ݜ��؜���
	virtual void OnStartScan();

	bool IsStarted(){ return m_bStarted; }

private:
	CScheduler( const CScheduler& );
	CScheduler& operator=( const CScheduler& );

	bool m_bStarted;

	//�������ޜ��ݜ�����՜��ٜ��ݜ��՜���, ������ޜ��Ԝ��՜������֜��М�����؜��� ���ޜ��ќ�����՜��ڜ������ ������ߜ������М��Ҝ��ۜ��՜��ݜ��؜��� ���М��Ӝ��՜��ݜ�����М��ܜ���, ���ל��М��Ӝ���������֜��М��՜��ܜ������ �������������М�����՜��Ӝ��؜��՜��� AgentsLoadStrategy
	std::map< std::string, SmartPtr< CAgentHandler > > m_mapAgentsContainer;
	CMutex m_mtxAgentsContainer;
	
	//���������؜��Ӝ��Ӝ��՜���, ����������М��ќ��М��������Ҝ��М��������؜��� ���ߜ������� ���ݜ��՜��ޜ��ќ�����ޜ��Ԝ��؜��ܜ��ޜ��������� ���ݜ��М�����М������ ������ڜ��М��ݜ��؜������ޜ��Ҝ��М��ݜ��؜���
	std::auto_ptr< CStartTrigger > m_pTrigger;

	class CListenThreadTask: public CThreadTask
	{
	public:

	CListenThreadTask( CScheduler* pScheduler ):
	  m_pScheduler( pScheduler )
	  {};
		  
	  virtual void Execute( CEvent& CancelEv );

	private:
		  
	  CScheduler* m_pScheduler;
	};

	CThread m_ListenThread;

	CEvent m_CloseEv;
	
	CServerSocket m_EventSock;

};

#endif //CSCHEDULER_H_
