#ifndef CTHREADPOOL_H_
#define CTHREADPOOL_H_

#include "SmartPtr.hpp"
#include "Event.hpp"
#include <vector>
#include "Semaphore.hpp"

class CThreadTask
{
public:
	virtual ~CThreadTask(){};

	virtual void Execute( const CEvent& CancelEvent ) = 0;

};

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

	class WorkingThread: public pt::thread
	{
	public:
		WorkingThread( CThreadsPool* pThis, int iId ):pt::thread(false),m_pThis(pThis),m_iId(iId){}
		virtual void execute();
		virtual void cleanup(){}

	private:
		CThreadsPool* m_pThis;
		int m_iId;
	};


	std::vector< SmartPtr< CEvent > > m_vecThreadsStates;

	pt::mutex m_mtThreadsStates;

	std::vector< SmartPtr<WorkingThread> > m_vecThreads;

	std::vector< SmartPtr< CThreadTask > > m_vecTasks;

	pt::mutex m_mtTasks;

	CEvent m_Cancel;

	CEvent m_Exit;

	CEvent m_TasksEmpty;

	pt::semaphore m_sem;

};

#endif
