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
        virtual void Execute( const CEvent& CancelEv );
	};

	CThread m_ProcessThread;

	CEvent m_CancelOpEv, m_TaskAddedEv;

	std::deque< SmartPtr< CTask > > m_deqTasks;

	CMutex m_mtxTasks, m_mtxCurState;
};


#endif /*TASKHANDLER_H_*/
