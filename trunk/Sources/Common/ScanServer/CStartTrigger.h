//-------------------------------------------------------------------------------------//
//Module: CStartTrigger interface and CCron, CTimer, CCalendar implementations
//Author: Parshin Dmitry
//Description: 
//-------------------------------------------------------------------------------------//
#ifndef CSTARTTRIGGER_H_
#define CSTARTTRIGGER_H_

#include "CScheduler.h"


class CStartScanEventInterface;

class CStartTrigger
{
public:

	CStartTrigger( CStartScanEventInterface* pCallBack );

	virtual ~CStartTrigger();
	
	virtual void Start() = 0;
	
	virtual void Stop() = 0;
	
protected:
	
	CStartScanEventInterface* m_pCallBack;
	
};

class CTimer: public CStartTrigger
{
public:

	CTimer( CStartScanEventInterface* pCallBack );

	virtual ~CTimer();
	
	void Start();
	
	void Stop();

private:
	CTimer( const CTimer& );
	CTimer& operator=( const CTimer& );
	
	CThread m_Thread;
	
	CEvent m_CancelEv;
	
	int m_iTimerValue;
	
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

class CCron: public CStartTrigger
{
public:

	CCron( CStartScanEventInterface* pCallBack );
	virtual ~CCron();
	
	void Start();
	
	void Stop();
private:
	CCron( const CCron& );
	CCron& operator=( const CCron& );
};

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
