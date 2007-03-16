//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
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
	
	//Конструктор
	CAgent();
	
	~CAgent(void);

private:

	
	CServerSocket m_sock;
	
	CClientSocket m_EventSock;
	
	//Адрес планировщика
	std::string m_strSchedulerAddress;

	//Событие закрытия
	CEvent m_CloseEvent;
	
	//Дескриптор потока ожидания соединений
	HANDLE m_hListenThread;
		
	//Поток ожидания входящих соединений
	static unsigned _stdcall fnListenThreadProc(  void* pParameter );
	
	std::vector< SmartPtr< CConnectionHandler > > m_vecConnections;
};

#endif
