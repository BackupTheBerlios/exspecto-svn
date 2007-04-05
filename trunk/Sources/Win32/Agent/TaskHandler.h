#ifndef TASKHANDLER_H_
#define TASKHANDLER_H_

#include "SmartPtr.hpp"
#include "CTask.h"
#include "Event.hpp"
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

	static unsigned _stdcall fnProcessThread( void* );
	
    HANDLE m_hProcessThread;
	
	CEvent m_CloseEv, m_CancelOpEv, m_TaskAddedEv;
	
	std::deque< SmartPtr< CTask > > m_deqTasks;
	
	CCriticalSection m_csTasks, m_csCurState;
};


#endif /*TASKHANDLER_H_*/
