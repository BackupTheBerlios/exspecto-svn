#ifndef TASKHANDLER_H_
#define TASKHANDLER_H_

#include "SmartPtr.hpp"
#include "CTask.h"
#include "Event.h"
#include <deque>

//-----------------------------------------------------------------------------------------------------------------
//---------------------------------------------CTaskHandler--------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

class CTaskHandler
{
public:

	CTaskHandler();
	virtual ~CTaskHandler();

	void AddTask( SmartPtr< CTask > pTask );

private:
	CTaskHandler( const CTaskHandler& );
	CTaskHandler& operator=( const CTaskHandler& );

	class CProcessThreadTask: public CThreadTask
	{
    public:

        CProcessThreadTask( CTaskHandler* pThis ):m_pThis( pThis )
        {}

        virtual void Execute( CEvent& CancelEv );

    private:

        CTaskHandler* m_pThis;
	};

	CThread m_ProcessThread;

	CEvent m_CancelOpEv, m_TaskAddedEv;

	std::deque< SmartPtr< CTask > > m_deqTasks;

	CMutex m_mtxTasks, m_mtxCurState;
};


#endif /*TASKHANDLER_H_*/
