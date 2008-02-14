//-------------------------------------------------------------------------------------//
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: 
//-------------------------------------------------------------------------------------//

#ifndef CSCHEDULER_H_
#define CSCHEDULER_H_
#include "ServerSocket.h"
#include "CAgentHandler.h"
#include "CStartTrigger.h"
#include "AgentsLoadStrategy.h"
#include "SmartPtr.hpp"
#include "Event.h"
#include "ThreadsPool.h"



class CStartTrigger;

class CStartScanEventInterface
{
public:

	virtual void OnStartScan() = 0;
};

class CScheduler: public CStartScanEventInterface 
{
public:
	CScheduler(void);
	virtual ~CScheduler(void);

	virtual void OnStartScan();

	bool IsStarted(){ return m_bStarted; }

private:
	CScheduler( const CScheduler& );
	CScheduler& operator=( const CScheduler& );

	bool m_bStarted;

	std::map< std::string, SmartPtr< CAgentHandler > > m_mapAgentsContainer;
	CMutex m_mtxAgentsContainer;
	
	std::auto_ptr< CStartTrigger > m_pTrigger;

	class CListenThreadTask: public CThreadTask
	{
	public:

	CListenThreadTask( CScheduler* pScheduler ):
	  m_pScheduler( pScheduler )
	  {};
		  
	  virtual void Execute( CEvent& CancelEv );

	private:
		  
	  CScheduler* m_pScheduler;
	};

	CThread m_ListenThread;

	CEvent m_CloseEv;
	
	CServerSocket m_EventSock;

};

#endif //CSCHEDULER_H_
