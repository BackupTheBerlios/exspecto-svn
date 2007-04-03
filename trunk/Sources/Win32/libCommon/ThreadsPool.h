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

	void WaitAllComplete( CEvent& CancelEv );

private:

	void SetCompleted( int iThreadId, BOOL bCompleted );

	SmartPtr< CThreadTask > GetTask();

	struct structParam{
		CThreadsPool* pThis;
		int	iId;
	};

	static unsigned __stdcall ThreadFunc( void* );

	std::vector< BOOL > m_vecThreadsState;

	CCriticalSection m_csThreadsStates;

	std::vector< SmartPtr< CThreadTask > > m_vecTasks;

	CCriticalSection m_csTasks;

	CEvent m_TaskAdded;

	CEvent m_Cancel;

	CEvent m_Exit;
};

#endif
