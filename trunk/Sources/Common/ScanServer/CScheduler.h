//-------------------------------------------------------------------------------------//
//œô¨Íœô¨âœô¨Þœô¨â œô¨äœô¨Ðœô¨Ùœô¨Û œô¨ïœô¨Òœô¨Ûœô¨ïœô¨Õœô¨âœô¨áœô¨ï œô¨çœô¨Ðœô¨áœô¨âœô¨ìœô¨î œô¨ßœô¨àœô¨Þœô¨Õœô¨Úœô¨âœô¨Ð Exspecto 2006œô¨Ó.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: œô¨ºœô¨Ûœô¨Ðœô¨áœô¨á, œô¨àœô¨Õœô¨Ðœô¨Ûœô¨Øœô¨×œô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨Ø œô¨ßœô¨Ûœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨éœô¨Øœô¨Úœô¨Ð
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



//œô¨¿œô¨àœô¨Õœô¨Ôœô¨Òœô¨Ðœô¨àœô¨Øœô¨âœô¨Õœô¨Ûœô¨ìœô¨Ýœô¨Þœô¨Õ œô¨Þœô¨Ñœô¨ìœô¨ïœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Úœô¨Ûœô¨Ðœô¨áœô¨áœô¨Ð CStartTrigger
class CStartTrigger;

//œô¨ºœô¨Ûœô¨Ðœô¨áœô¨á, œô¨Þœô¨ßœô¨Øœô¨áœô¨ëœô¨Òœô¨Ðœô¨îœô¨éœô¨Øœô¨Ù œô¨Øœô¨Ýœô¨âœô¨Õœô¨àœô¨äœô¨Õœô¨Ùœô¨á œô¨Ôœô¨Ûœô¨ï œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨Ø œô¨Þœô¨Ñœô¨àœô¨Ðœô¨âœô¨Ýœô¨Þœô¨Óœô¨Þ œô¨Òœô¨ëœô¨×œô¨Þœô¨Òœô¨Ð OnStartScanEvent,
//œô¨Òœô¨ëœô¨×œô¨ëœô¨Òœô¨Ðœô¨Õœô¨Üœô¨Þœô¨Ù œô¨Ò CScheduler œô¨ßœô¨àœô¨Ø œô¨áœô¨àœô¨Ðœô¨Ñœô¨Ðœô¨âœô¨ëœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Ø œô¨âœô¨àœô¨Øœô¨Óœô¨Óœô¨Õœô¨àœô¨Ð CStartTrigger
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

	//œô¨¼œô¨Õœô¨âœô¨Þœô¨Ô œô¨Òœô¨ëœô¨×œô¨ëœô¨Òœô¨Ðœô¨Õœô¨âœô¨áœô¨ï œô¨âœô¨àœô¨Øœô¨Óœô¨Óœô¨Õœô¨àœô¨Þœô¨Ü œô¨ßœô¨àœô¨Ø œô¨áœô¨àœô¨Ðœô¨Ñœô¨Ðœô¨âœô¨ëœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Ø
	virtual void OnStartScan();

	bool IsStarted(){ return m_bStarted; }

private:
	CScheduler( const CScheduler& );
	CScheduler& operator=( const CScheduler& );

	bool m_bStarted;

	//œô¨ºœô¨Þœô¨Ýœô¨âœô¨Õœô¨Ùœô¨Ýœô¨Õœô¨à, œô¨áœô¨Þœô¨Ôœô¨Õœô¨àœô¨Öœô¨Ðœô¨éœô¨Øœô¨Ù œô¨Þœô¨Ñœô¨ìœô¨Õœô¨Úœô¨âœô¨ë œô¨ãœô¨ßœô¨àœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨Øœô¨ï œô¨Ðœô¨Óœô¨Õœô¨Ýœô¨âœô¨Ðœô¨Üœô¨Ø, œô¨×œô¨Ðœô¨Óœô¨àœô¨ãœô¨Öœô¨Ðœô¨Õœô¨Üœô¨ëœô¨Õ œô¨áœô¨âœô¨àœô¨Ðœô¨âœô¨Õœô¨Óœô¨Øœô¨Õœô¨Ù AgentsLoadStrategy
	std::map< std::string, SmartPtr< CAgentHandler > > m_mapAgentsContainer;
	CMutex m_mtxAgentsContainer;
	
	//œô¨Âœô¨àœô¨Øœô¨Óœô¨Óœô¨Õœô¨à, œô¨áœô¨àœô¨Ðœô¨Ñœô¨Ðœô¨âœô¨ëœô¨Òœô¨Ðœô¨îœô¨éœô¨Øœô¨Ù œô¨ßœô¨àœô¨Ø œô¨Ýœô¨Õœô¨Þœô¨Ñœô¨åœô¨Þœô¨Ôœô¨Øœô¨Üœô¨Þœô¨áœô¨âœô¨Ø œô¨Ýœô¨Ðœô¨çœô¨Ðœô¨âœô¨ì œô¨áœô¨Úœô¨Ðœô¨Ýœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨Õ
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
