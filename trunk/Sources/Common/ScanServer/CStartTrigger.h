//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CStartTrigger interface and CCron, CTimer, CCalendar implementations
//Author: Parshin Dmitry
//Description: �����, ����������� ������� �������� ��� ������������ CScheduler
//-------------------------------------------------------------------------------------//
#ifndef CSTARTTRIGGER_H_
#define CSTARTTRIGGER_H_

#include "CScheduler.h"


//��������������� ���������� ������ CStartScanEventInterface
class CStartScanEventInterface;

//�����, ����������� ��������� �������� ��� ������������ CScheduler
class CStartTrigger
{
public:
	//������� ��������� � ���������� �� ��������� ��������� ������,
	//�������� ����� ������������ �������
	CStartTrigger( CStartScanEventInterface* pCallBack );
	virtual ~CStartTrigger();
	
	//������ ������ ��������
	virtual void Start() = 0;
	
	//������� � ����� ��������
	virtual void Stop() = 0;
	
protected:
	
	CStartScanEventInterface* m_pCallBack;
	
};

//�����,����������� �������, ������������� ������������ �� �������
//�������� ������� ������� ������� �� ���������
class CTimer: public CStartTrigger
{
public:

	CTimer( CStartScanEventInterface* pCallBack );
	~CTimer();
	
	//��������� ������
	void Start();
	
	//���������� � �������� ������
	void Stop();

private:
	CTimer( const CTimer& );
	CTimer& operator=( const CTimer& );
	
	//���������� ������ �������
	HANDLE m_hThread;
	
	//���������� �������, ���������������� ����� �������
	HANDLE m_hCancelEvent;
	
	//�������� ������� �������
	int m_iTimerValue;
	
	//����� �������
	static unsigned __stdcall fnTimerProc( void* pParam );
 
};

//�����,����������� �������, ������������� �� ������� �� ������� ��������� 
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

//�����,����������� �������, ������������� �� ����������
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