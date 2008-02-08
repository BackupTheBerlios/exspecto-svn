//-------------------------------------------------------------------------------------//
//���͜�����ޜ��� ������М��ٜ��� ������Ҝ��ۜ�����՜��������� ������М������������ ���ߜ������ޜ��՜��ڜ������ Exspecto 2006����.
//Module: CStartTrigger interface and CCron, CTimer, CCalendar implementations
//Author: Parshin Dmitry
//Description: �������ۜ��М������, �������՜��М��ۜ��؜��ל�����������؜��� ���������ݜ��ڜ�����؜��� ����������؜��Ӝ��Ӝ��՜������� ���Ԝ��ۜ��� ���ߜ��ۜ��М��ݜ��؜������ޜ��Ҝ�����؜��ڜ��� CScheduler
//-------------------------------------------------------------------------------------//
#ifndef CSTARTTRIGGER_H_
#define CSTARTTRIGGER_H_

#include "CScheduler.h"


//�����������՜��Ԝ��Ҝ��М������؜�����՜��ۜ�����ݜ��ޜ��� ���ޜ��ќ��������Ҝ��ۜ��՜��ݜ��؜��� ���ڜ��ۜ��М��������� CStartScanEventInterface
class CStartScanEventInterface;

//�������ۜ��М������, ���ޜ��ߜ��؜��������Ҝ��М��������؜��� ���؜��ݜ�����՜���������՜��ٜ��� ����������؜��Ӝ��Ӝ��՜������� ���Ԝ��ۜ��� ���ߜ��ۜ��М��ݜ��؜������ޜ��Ҝ�����؜��ڜ��� CScheduler
class CStartTrigger
{
public:
	//���������؜��Ӝ��Ӝ��՜��� ������ޜ��ל��Ԝ��М��՜��������� ���� ������ڜ��М��ל��М�����՜��ۜ��՜��� ���ݜ��� ���؜��ݜ�����՜���������՜��ٜ��� ���ޜ��ќ������М�����ݜ��ޜ��Ӝ��� ���Ҝ�����ל��ޜ��Ҝ���,
	//���ڜ��ޜ�����ޜ������ޜ��ܜ��� ���ќ�����Ԝ��՜��� ���ߜ��՜������՜��Ԝ��М��Ҝ��М������������ ������ޜ��ќ��������؜���
	CStartTrigger( CStartScanEventInterface* pCallBack );
	virtual ~CStartTrigger();
	
	//�������М�����М��ۜ��� �������М��ќ��ޜ������ ����������؜��Ӝ��Ӝ��՜�������
	virtual void Start() = 0;
	
	//�������������М��ݜ��ޜ��� ���� ������ќ������ޜ��� ����������؜��Ӝ��Ӝ��՜�������
	virtual void Stop() = 0;
	
protected:
	
	CStartScanEventInterface* m_pCallBack;
	
};

//�������ۜ��М������,�������՜��М��ۜ��؜��ל�����������؜��� ����������؜��Ӝ��Ӝ��՜���, ����������М��ќ��М��������Ҝ��М��������؜��� ���ߜ��՜������؜��ޜ��Ԝ��؜�����՜�����ڜ��� ���ߜ��� ������М��ٜ��ܜ��՜�������
//�������ݜ��М�����՜��ݜ��؜��� ���ߜ��՜������؜��ޜ��Ԝ��� ������М��ٜ��ܜ��՜������� ���ќ��՜������՜��������� ���؜��� ������������М��ݜ��ޜ��Ҝ��ޜ���
class CTimer: public CStartTrigger
{
public:

	CTimer( CStartScanEventInterface* pCallBack );
	~CTimer();
	
	//�������М��ߜ�����������؜������ ������М��ٜ��ܜ��՜���
	void Start();
	
	//�������������М��ݜ��ޜ��Ҝ��؜������ ���� ������ќ������ޜ�����؜������ ������М��ٜ��ܜ��՜���
	void Stop();

private:
	CTimer( const CTimer& );
	CTimer& operator=( const CTimer& );
	
	//�������՜�����ڜ������؜��ߜ�����ޜ��� ���ߜ��ޜ�����ޜ��ڜ��� ������М��ٜ��ܜ��՜�������
	CThread m_Thread;
	
	//�������՜�����ڜ������؜��ߜ�����ޜ��� ������ޜ��ќ��������؜���, ���ޜ��������М��ݜ��М��Ҝ��ۜ��؜��Ҝ��М��������՜��Ӝ��� ���ߜ��ޜ�����ޜ��� ������М��ٜ��ܜ��՜�������
	CEvent m_CancelEv;
	
	//�������ݜ��М�����՜��ݜ��؜��� ���ߜ��՜������؜��ޜ��Ԝ��� ������М��ٜ��ܜ��՜�������
	int m_iTimerValue;
	
	//�������ޜ�����ޜ��� ������М��ٜ��ܜ��՜�������
	class CTimerThreadTask: public CThreadTask
	{
	public:

	CTimerThreadTask( CTimer* pTimer):
	  m_pTimer( pTimer )
	  {}
		  
	  virtual void Execute( CEvent& CancelEv );

	private:
		  
	  CTimer* m_pTimer;
	};

 
};

//�������ۜ��М������,�������՜��М��ۜ��؜��ל�����������؜��� ����������؜��Ӝ��Ӝ��՜���, ����������М��ќ��М��������Ҝ��М��������؜��� ���ߜ��� ������ޜ��ќ��������؜��� ���ޜ��� ���Ҝ��ݜ��՜�����ݜ��՜��� ���ߜ������ޜ��Ӝ������М��ܜ��ܜ��� 
class CCron: public CStartTrigger
{
public:

	CCron( CStartScanEventInterface* pCallBack );
	~CCron();
	
	void Start();
	
	void Stop();
private:
	CCron( const CCron& );
	CCron& operator=( const CCron& );
};

//�������ۜ��М������,�������՜��М��ۜ��؜��ל�����������؜��� ����������؜��Ӝ��Ӝ��՜���, ����������М��ќ��М��������Ҝ��М��������؜��� ���ߜ��� �������М�����ߜ��؜�����М��ݜ��؜���
class CCalendar: public CStartTrigger
{
public:

	CCalendar( CStartScanEventInterface* pCallBack );
	~CCalendar();
	
	void Start();
	
	void Stop();
private:
	CCalendar( const CCalendar& );
	CCalendar& operator=( const CCalendar& );
};

#endif /*CSTARTTRIGGER_H_*/
