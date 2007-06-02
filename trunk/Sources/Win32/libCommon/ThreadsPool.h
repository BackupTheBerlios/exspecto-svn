#ifndef CTHREADPOOL_H_
#define CTHREADPOOL_H_

#include "SmartPtr.hpp"
#include "CriticalSection.hpp"
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

//	bool WaitAllComplete( int iTimeout = -1 );

	bool WaitAllComplete( CEvent& CancelEv );

private:

	CThreadsPool( const CThreadsPool& );
	CThreadsPool& operator=( const CThreadsPool& );

	void LogStates();

	void SetCompleted( int iThreadId, bool bCompleted );

	SmartPtr< CThreadTask > GetTask();

	struct structParam{
		CThreadsPool* pThis;
		int	iId;
	};

	static unsigned __stdcall ThreadFunc( void* );

	std::vector< SmartPtr< CEvent > > m_vecThreadsStates;

	CCriticalSection m_csThreadsStates;

	std::vector< HANDLE > m_vecThreads;

	std::vector< SmartPtr< CThreadTask > > m_vecTasks;

	CCriticalSection m_csTasks;

	CSemaphore m_TasksSem;

	CEvent m_Cancel;

	CEvent m_Exit;

	CEvent m_TasksEmpty;
};

#endif
