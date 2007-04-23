//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#ifndef AGENT_H_
#define AGENT_H_

#include "ServerSocket.h"
#include "ClientSocket.h"
#include "packet.h"
#include "Container.hpp"
#include "CScanner.h"
#include "PluginLoadStrategy.h"
#include <set>

#include <map>
#include <vector>
#include <deque>
#include <string>
#include "SmartPtr.hpp"
#include "CriticalSection.hpp"
#include "Event.hpp" 
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
	
	//����� ������������
	std::string m_strSchedulerAddress;

	//������� ��������
	CEvent m_CloseEvent;
	
	//���������� ������ �������� ����������
	HANDLE m_hListenThread;
		
	//����� �������� �������� ����������
	static unsigned _stdcall fnListenThreadProc(  void* pParameter );

};

#endif
