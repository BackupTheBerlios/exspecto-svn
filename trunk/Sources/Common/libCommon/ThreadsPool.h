#ifndef CTHREADPOOL_H_
#define CTHREADPOOL_H_

#include "SmartPtr.hpp"
#include "Event.h"
#include <vector>
#include "Semaphore.h"
#include "Thread.h"


class CThreadsPool
{
public:
	CThreadsPool( int iThreadsCount );

	virtual ~CThreadsPool(void);

	void AddTask( SmartPtr< CThreadTask > pTask );

	void CancelAllTasks();

	bool WaitAllComplete( CEvent& CancelEv );

private:

	CThreadsPool( const CThreadsPool& );
	CThreadsPool& operator=( const CThreadsPool& );

	void LogStates();

	void SetCompleted( int iThreadId, bool bCompleted );

	SmartPtr< CThreadTask > GetTask();

	class WorkingThreadTask: public CThreadTask
	{
	public:
		WorkingThreadTask( CThreadsPool* pThis, int iId ):m_pThis(pThis),m_iId(iId){}
		virtual void Execute( CEvent& CancelEvent );

	private:
		CThreadsPool* m_pThis;
		int m_iId;
	};


	std::vector< SmartPtr< CEvent > > m_vecThreadsStates;

	CMutex m_mtThreadsStates;

	std::vector< SmartPtr<CThread> > m_vecThreads;

	std::vector< SmartPtr< CThreadTask > > m_vecTasks;

	CMutex m_mtTasks;

	CEvent m_Cancel;

	CEvent m_Exit;

	CEvent m_TasksEmpty;

	CSemaphore m_sem;

};

#endif
