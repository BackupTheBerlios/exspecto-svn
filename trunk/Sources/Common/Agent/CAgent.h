//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
//-------------------------------------------------------------------------------------//
#ifndef AGENT_H_
#define AGENT_H_

#include "ServerSocket.h"
#include "ClientSocket.h"
#include "Packet.h"
#include "CScanner.h"
#include <set>

#include <map>
#include <vector>
#include <deque>
#include <string>
#include "SmartPtr.hpp"
#include "Event.h"
#include "CTask.h"
#include "ServerHandler.h"
#include "ConnectionHandler.h"


//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------CAgent------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------


class CAgent
{
public:

	CAgent();

	~CAgent(void);

	bool IsStarted(){ return m_bStarted; }

private:

	CAgent( const CAgent& );
	CAgent& operator=( const CAgent& );

	bool m_bStarted;

	std::vector< SmartPtr< CConnectionHandler > > m_vecConnections;

	SmartPtr< CServerSocket > m_pMsgSock;

	SmartPtr< CClientSocket > m_pEventSock;

	//Адрес планировщика
	std::string m_strSchedulerAddress;

	class CListenThreadTask: public CThreadTask
	{
    public:

        CListenThreadTask( CAgent* pAgent ):m_pAgent( pAgent ){}

        virtual void Execute( CEvent& CancelEv );
    private:

        CAgent* m_pAgent;
	};
	CThread m_ListenThread;
};

#endif
