#ifndef CTHREADPOOL_H_
#define CTHREADPOOL_H_

#include "SmartPtr.hpp"
#include "CriticalSection.hpp"
#include "Event.hpp"
#include <vector>

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

	bool WaitAllComplete( int iTimeout = -1 );

//	void WaitAllComplete( CEvent& CancelEv );

private:

	void SetCompleted( int iThreadId, bool bCompleted );

	SmartPtr< CThreadTask > GetTask();

	struct structParam{
		CThreadsPool* pThis;
		int	iId;
	};

	static unsigned __stdcall ThreadFunc( void* );

	std::vector< SmartPtr< CEvent > > m_vecThreadsStates;

	CCriticalSection m_csThreadsStates;

	std::vector< HANDLE > m_vecStateHandles;

	std::vector< SmartPtr< CThreadTask > > m_vecTasks;

	CCriticalSection m_csTasks;

	CEvent m_TaskAdded;

	CEvent m_Cancel;

	CEvent m_Exit;
};

#endif
