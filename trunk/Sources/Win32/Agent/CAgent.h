//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#ifndef AGENT_H_
#define AGENT_H_

#include "precomp.h"
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
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "CTask.h"
#include "ServerHandler.h"
#include "ConnectionHandler.h"


//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------CAgent------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------


class CAgent
{
public:
	
	//�����������
	CAgent();
	
	~CAgent(void);

private:

	
	CServerSocket m_sock;
	
	CClientSocket m_EventSock;
	
	//����� ������������
	std::string m_strSchedulerAddress;

	//������� ��������
	CEvent m_CloseEvent;
	
	//���������� ������ �������� ����������
	HANDLE m_hListenThread;
		
	//����� �������� �������� ����������
	static unsigned _stdcall fnListenThreadProc(  void* pParameter );
	
	std::vector< SmartPtr< CConnectionHandler > > m_vecConnections;
};

#endif
